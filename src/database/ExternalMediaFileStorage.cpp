#include "src/database/ExternalMediaFileStorage.h"

#include "src/backup/ContactMediaItemBackupObject.h"
#include "src/backup/GroupMediaItemBackupObject.h"
#include "src/crypto/Crc32.h"
#include "src/database/Database.h"
#include "src/database/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QUuid>
#include <QSqlQuery>

#include <sodium.h>

namespace openmittsu {
	namespace database {

		ExternalMediaFileStorage::ExternalMediaFileStorage(QDir const& storagePath, openmittsu::database::Database& database) : MediaFileStorage(), m_storagePath(storagePath), m_database(database) {
			//
		}
		
		ExternalMediaFileStorage::~ExternalMediaFileStorage() {
			//
		}

		QString ExternalMediaFileStorage::buildFilename(QString const& uuid) const {
			return QStringLiteral("encMedia_1_").append(uuid);
		}

		bool ExternalMediaFileStorage::hasMediaItem(QString const& uuid) const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `uid` FROM `media` WHERE `uid` = :uuid"));
			query.bindValue(QStringLiteral(":uuid"), QVariant(uuid));

			if (query.exec() && query.isSelect()) {
				return query.next();
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute media existance query for uuid \"" << uuid.toStdString() << "\" table 'media'. Query error: " << query.lastError().text().toStdString();
			}
		}
		
		int ExternalMediaFileStorage::getMediaItemCount() const {
			return DatabaseUtilities::countQuery(m_database.database, QStringLiteral("media"));
		}

		MediaFileItem ExternalMediaFileStorage::getMediaItem(QString const& uuid) const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `uid`, `size`, `checksum`, `nonce`, `key` FROM `media` WHERE `uid` = :uuid"));
			query.bindValue(QStringLiteral(":uuid"), QVariant(uuid));

			if (query.exec() && query.isSelect() && query.next()) {
				int const size = query.value(QStringLiteral("size")).toInt();
				uint32_t const specifiedChecksum = query.value(QStringLiteral("checksum")).toUInt();
				QString const nonceString = query.value(QStringLiteral("nonce")).toString();
				QByteArray const nonce = QByteArray::fromHex(nonceString.toUtf8());
				if (nonce.size() != cryptoGetNonceSize()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not fetch media item for uuid \"" << uuid.toStdString() << "\". The nonce size is incorrect.";
				}
				QString const keyString = query.value(QStringLiteral("key")).toString();
				QByteArray const key = QByteArray::fromHex(keyString.toUtf8());
				if (key.size() != cryptoGetKeySize()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not fetch media item for uuid \"" << uuid.toStdString() << "\". The key size is incorrect.";
				}

				QFile file(m_storagePath.filePath(buildFilename(uuid)));
				if (!file.open(QFile::ReadOnly)) {
					LOGGER()->warn("Could not fetch media item for uuid \"{}\". Could not open or read file.", uuid.toStdString());
					return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_EXTERNAL_FILE_DELETED);
				}
				
				QByteArray const data = file.readAll();
				file.close();

				QByteArray const decryptedData = decrypt(data, key, nonce);
				if (decryptedData.size() != size) {
					LOGGER()->warn("Could not fetch media item for uuid \"{}\". File size {} Bytes does not match expected size of {} Bytes!", uuid.toStdString(), decryptedData.size(), size);
					return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_DECRYPTION_FAILED);
				} else if(specifiedChecksum != openmittsu::crypto::Crc32::checksum(decryptedData)) {
					LOGGER()->warn("Could not fetch media item for uuid \"{}\". The specified checksum {} did not match the checksum of the retrieved object {}.", uuid.toStdString(), openmittsu::crypto::Crc32::toString(specifiedChecksum).toStdString(), openmittsu::crypto::Crc32::toString(openmittsu::crypto::Crc32::checksum(decryptedData)).toStdString());
					return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_FILE_CORRUPTED);
				}

				return MediaFileItem(decryptedData);
			} else {
				LOGGER()->warn("Could not execute media query for uuid \"{}\" table 'media'. Query error: {}", uuid.toStdString(), query.lastError().text().toStdString());
				return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE);
			}
		}

		void ExternalMediaFileStorage::insertMediaItem(QString const& uuid, QByteArray const& data) {
			int const size = data.size();
			uint32_t actualChecksum = openmittsu::crypto::Crc32::checksum(data);

			QByteArray const key = generateKey();
			QByteArray const nonce = generateNonce();
			QByteArray const encryptedData = encrypt(data, key, nonce);
			int const encryptedSize = data.size() + cryptoGetHeaderSize();

			QFile file(m_storagePath.filePath(buildFilename(uuid)));
			if (!file.open(QFile::WriteOnly)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not write media item for uuid \"" << uuid.toStdString() << "\". Could not open file for writing.";
			}
			
			qint64 const writtenBytes = file.write(encryptedData);
			if (writtenBytes != encryptedSize) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not write media item for uuid \"" << uuid.toStdString() << "\". Could not write file data (size missmatch, " << writtenBytes << " vs. " << encryptedSize << " Bytes).";
			}
			file.close();

			QSqlQuery queryMedia(m_database.database);
			queryMedia.prepare(QStringLiteral("INSERT INTO `media` (`uid`, `size`, `checksum`, `nonce`, `key`) VALUES (:uid, :size, :checksum, :nonce, :key);"));
			queryMedia.bindValue(QStringLiteral(":uid"), QVariant(uuid));
			queryMedia.bindValue(QStringLiteral(":size"), QVariant(size));
			queryMedia.bindValue(QStringLiteral(":checksum"), QVariant(actualChecksum));
			queryMedia.bindValue(QStringLiteral(":nonce"), QVariant(QString(nonce.toHex())));
			queryMedia.bindValue(QStringLiteral(":key"), QVariant(QString(key.toHex())));
			if (!queryMedia.exec()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not insert media data into 'media'. Query error: " << queryMedia.lastError().text().toStdString();
			}
		}
		
		QString ExternalMediaFileStorage::insertMediaItem(QByteArray const& data) {
			QString const uuid = m_database.generateUuid();
			insertMediaItem(uuid, data);

			return uuid;
		}
		
		void ExternalMediaFileStorage::removeMediaItem(QString const& uuid) {
			QFile::remove(m_storagePath.filePath(buildFilename(uuid)));

			QSqlQuery queryMedia(m_database.database);
			queryMedia.prepare(QStringLiteral("DELETE FROM `media` WHERE `uid` = :uuid;"));
			queryMedia.bindValue(QStringLiteral(":uuid"), QVariant(uuid));
			if (!queryMedia.exec()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not delete media data from table 'media'. Query error: " << queryMedia.lastError().text().toStdString();
			}
		}

		int ExternalMediaFileStorage::cryptoGetNonceSize() const {
			return crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
		}

		int ExternalMediaFileStorage::cryptoGetHeaderSize() const {
			return crypto_aead_xchacha20poly1305_ietf_ABYTES;
		}
		
		int ExternalMediaFileStorage::cryptoGetKeySize() const {
			return crypto_aead_xchacha20poly1305_ietf_KEYBYTES;
		}

		QByteArray ExternalMediaFileStorage::decrypt(QByteArray const& encryptedData, QByteArray const& key, QByteArray const& nonce) const {
			QByteArray decryptedData(encryptedData.size() - cryptoGetHeaderSize(), '\0');
			unsigned long long decrypted_len;
			if (crypto_aead_xchacha20poly1305_ietf_decrypt(reinterpret_cast<unsigned char*>(decryptedData.data()), &decrypted_len, NULL, reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), NULL, 0, reinterpret_cast<unsigned char const*>(nonce.data()), reinterpret_cast<unsigned char const*>(key.data())) != 0) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not decrypt blob, data corrupt!";
			} else if (static_cast<int>(decrypted_len) != (encryptedData.size() - cryptoGetHeaderSize())) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not decrypt media item blob: Expected size was " << (encryptedData.size() - cryptoGetHeaderSize()) << " Bytes, but we got " << decrypted_len << " Bytes instead!";
			}

			return decryptedData;
		}

		QByteArray ExternalMediaFileStorage::generateKey() const {
			QByteArray keyBytes(cryptoGetKeySize(), '\0');
			crypto_aead_xchacha20poly1305_ietf_keygen(reinterpret_cast<unsigned char*>(keyBytes.data()));
			return keyBytes;
		}

		QByteArray ExternalMediaFileStorage::generateNonce() const {
			QByteArray nonceBytes(cryptoGetNonceSize(), '\0');
			randombytes_buf(nonceBytes.data(), cryptoGetNonceSize());
			return nonceBytes;
		}
		
		QByteArray ExternalMediaFileStorage::encrypt(QByteArray const& unencryptedData, QByteArray const& key, QByteArray const& nonce) const {
			QByteArray encryptedData(unencryptedData.size() + cryptoGetHeaderSize(), '\0');
			unsigned long long ciphertext_len;
			if (crypto_aead_xchacha20poly1305_ietf_encrypt(reinterpret_cast<unsigned char*>(encryptedData.data()), &ciphertext_len, reinterpret_cast<unsigned char const*>(unencryptedData.data()), unencryptedData.size(), NULL, 0, NULL, reinterpret_cast<unsigned char const*>(nonce.data()), reinterpret_cast<unsigned char const*>(key.data())) != 0) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not encrypt blob, data corrupt!";
			} else if (static_cast<int>(ciphertext_len) != (unencryptedData.size() + cryptoGetHeaderSize())) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not encrypt media item blob: Expected size was " << (unencryptedData.size() + cryptoGetHeaderSize()) << " Bytes, but we got " << ciphertext_len << " Bytes instead!";
			}

			return encryptedData;
		}

		void ExternalMediaFileStorage::insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) {
			if (!m_database.database.transaction()) {
				LOGGER()->warn("ExternalMediaFileStorage: Could NOT start transaction!");
			}

			auto it = items.constBegin();
			auto end = items.constEnd();
			for (; it != end; ++it) {
				insertMediaItem(it->getUuid(), it->getData());
			}

			if (!m_database.database.commit()) {
				LOGGER()->info("ExternalMediaFileStorage: Could NOT commit transaction!");
			}
		}

		void ExternalMediaFileStorage::insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) {
			if (!m_database.database.transaction()) {
				LOGGER()->warn("ExternalMediaFileStorage: Could NOT start transaction!");
			}

			auto it = items.constBegin();
			auto end = items.constEnd();
			for (; it != end; ++it) {
				insertMediaItem(it->getUuid(), it->getData());
			}

			if (!m_database.database.commit()) {
				LOGGER()->info("ExternalMediaFileStorage: Could NOT commit transaction!");
			}
		}

	}
}
