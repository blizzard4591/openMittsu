#include "src/database/internal/ExternalMediaFileStorage.h"

#include "src/backup/ContactMediaItemBackupObject.h"
#include "src/backup/GroupMediaItemBackupObject.h"
#include "src/crypto/Crc32.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/UuidAlreadyExistsException.h"
#include "src/utility/Logging.h"

#include <QDirIterator>
#include <QUuid>
#include <QSqlQuery>
#include <QRegularExpression>

#include <sodium.h>

namespace openmittsu {
	namespace database {
		namespace internal {

			ExternalMediaFileStorage::ExternalMediaFileStorage(QDir const& storagePath, InternalDatabaseInterface* database) : MediaFileStorage(), m_storagePath(storagePath), m_database(database) {
				//
			}

			ExternalMediaFileStorage::~ExternalMediaFileStorage() {
				//
			}

			void ExternalMediaFileStorage::upgradeMediaDatabase(int fromVersion) {
				if (fromVersion < 2) {
					// Move all files to follow new type filename scheme
					QRegularExpression regex("^encMedia_1_([^_]+)$");
					QDir dir(m_storagePath.absolutePath());
					QDirIterator it(dir.absolutePath(), QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
					while (it.hasNext()) {
						it.next();
						QRegularExpressionMatch match = regex.match(it.fileName());
						if (match.hasMatch()) {
							dir.rename(it.fileName(), QStringLiteral("encMedia_1_1_").append(match.captured(1)));
						}
					}
					fromVersion = 2;
				}
			}

			QString ExternalMediaFileStorage::buildFilename(QString const& uuid, MediaFileType const& fileType) const {
				return QStringLiteral("encMedia_1_%1_").arg(MediaFileTypeHelper::toInt(fileType)).append(uuid);
			}

			bool ExternalMediaFileStorage::hasMediaItem(QString const& uuid, MediaFileType const& fileType) const {
				QSqlQuery query(m_database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `uid` FROM `media` WHERE `uid` = :uuid AND `type` = :type"));
				query.bindValue(QStringLiteral(":uuid"), QVariant(uuid));
				query.bindValue(QStringLiteral(":type"), QVariant(MediaFileTypeHelper::toInt(fileType)));

				if (query.exec() && query.isSelect()) {
					return query.next();
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute media existance query for uuid \"" << uuid.toStdString() << "\" table 'media'. Query error: " << query.lastError().text().toStdString();
				}
			}

			int ExternalMediaFileStorage::getMediaItemCount() const {
				return DatabaseUtilities::countQuery(m_database, QStringLiteral("media"));
			}

			MediaFileItem ExternalMediaFileStorage::getMediaItem(QString const& uuid, MediaFileType const& fileType) const {
				QSqlQuery query(m_database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `uid`, `size`, `checksum`, `nonce`, `key` FROM `media` WHERE `uid` = :uuid AND `type` = :type"));
				query.bindValue(QStringLiteral(":uuid"), QVariant(uuid));
				query.bindValue(QStringLiteral(":type"), QVariant(MediaFileTypeHelper::toInt(fileType)));

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

					QFile file(m_storagePath.filePath(buildFilename(uuid, fileType)));
					if (!file.open(QFile::ReadOnly)) {
						LOGGER()->warn("Could not fetch media item for uuid \"{}\". Could not open or read file.", uuid.toStdString());
						return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_EXTERNAL_FILE_DELETED, fileType);
					}

					QByteArray const data = file.readAll();
					file.close();

					QByteArray const decryptedData = decrypt(data, key, nonce);
					if (decryptedData.size() != size) {
						LOGGER()->warn("Could not fetch media item for uuid \"{}\". File size {} Bytes does not match expected size of {} Bytes!", uuid.toStdString(), decryptedData.size(), size);
						return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_DECRYPTION_FAILED, fileType);
					} else if (specifiedChecksum != openmittsu::crypto::Crc32::checksum(decryptedData)) {
						LOGGER()->warn("Could not fetch media item for uuid \"{}\". The specified checksum {} did not match the checksum of the retrieved object {}.", uuid.toStdString(), openmittsu::crypto::Crc32::toString(specifiedChecksum).toStdString(), openmittsu::crypto::Crc32::toString(openmittsu::crypto::Crc32::checksum(decryptedData)).toStdString());
						return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_FILE_CORRUPTED, fileType);
					}

					return MediaFileItem(decryptedData, fileType);
				} else {
					LOGGER()->warn("Could not execute media query for uuid \"{}\" table 'media'. Query error: {}", uuid.toStdString(), query.lastError().text().toStdString());
					return MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE, fileType);
				}
			}

			void ExternalMediaFileStorage::insertMediaItem(QString const& uuid, QByteArray const& data, MediaFileType const& fileType) {
				int const size = data.size();
				uint32_t actualChecksum = openmittsu::crypto::Crc32::checksum(data);

				QByteArray const key = generateKey();
				QByteArray const nonce = generateNonce();
				QByteArray const encryptedData = encrypt(data, key, nonce);
				int const encryptedSize = data.size() + cryptoGetHeaderSize();

				QFile file(m_storagePath.filePath(buildFilename(uuid, fileType)));
				if (!file.open(QFile::WriteOnly)) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not write media item for uuid \"" << uuid.toStdString() << "\". Could not open file for writing.";
				}

				qint64 const writtenBytes = file.write(encryptedData);
				if (writtenBytes != encryptedSize) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not write media item for uuid \"" << uuid.toStdString() << "\". Could not write file data (size missmatch, " << writtenBytes << " vs. " << encryptedSize << " Bytes).";
				}
				file.close();

				QSqlQuery queryMedia(m_database->getQueryObject());
				queryMedia.prepare(QStringLiteral("INSERT INTO `media` (`uid`, `type`, `size`, `checksum`, `nonce`, `key`) VALUES (:uid, :type, :size, :checksum, :nonce, :key);"));
				queryMedia.bindValue(QStringLiteral(":uid"), QVariant(uuid));
				queryMedia.bindValue(QStringLiteral(":type"), QVariant(MediaFileTypeHelper::toInt(fileType)));
				queryMedia.bindValue(QStringLiteral(":size"), QVariant(size));
				queryMedia.bindValue(QStringLiteral(":checksum"), QVariant(actualChecksum));
				queryMedia.bindValue(QStringLiteral(":nonce"), QVariant(QString(nonce.toHex())));
				queryMedia.bindValue(QStringLiteral(":key"), QVariant(QString(key.toHex())));
				if (!queryMedia.exec()) {
					auto const queryErrorMessage = queryMedia.lastError().text();
					if (queryErrorMessage.contains("UNIQUE constraint failed: media.uid")) {
						QSqlQuery query(m_database->getQueryObject());
						query.prepare(QStringLiteral("SELECT `uid`, `type`, `size`, `checksum` FROM `media` WHERE `uid` = :uuid AND `type` = :type"));
						query.bindValue(QStringLiteral(":uuid"), QVariant(uuid));
						query.bindValue(QStringLiteral(":type"), QVariant(MediaFileTypeHelper::toInt(fileType)));

						if (query.exec() && query.isSelect()) {
							if (query.next()) {
								int const storedType = query.value(QStringLiteral("type")).toInt();
								int const storedSize = query.value(QStringLiteral("size")).toInt();
								uint32_t const storedChecksum = query.value(QStringLiteral("checksum")).toUInt();
								if ((storedType == MediaFileTypeHelper::toInt(fileType)) && (storedSize == size) && (storedChecksum == actualChecksum)) {
									LOGGER()->warn("Media item with this UUID and properties already exists in database, ignoring...");
									// Hmpf, okay, file already stored, so, whatever...
									return;
								} else {
									throw openmittsu::exceptions::UuidAlreadyExistsException() << "Could not insert media data into 'media', item with this UUID already exists, but size ('" << storedSize << "' vs. '" << size << "'), type ('" << storedType << "' vs. '" << MediaFileTypeHelper::toInt(fileType) << "') or checksum ('" << storedChecksum << "' vs. '" << actualChecksum << "') do not match! Query error: " << queryErrorMessage.toStdString();
								}
							} else {
								throw openmittsu::exceptions::UuidAlreadyExistsException() << "Could not insert media data into 'media', item with this UUID already exists, but check query came up empty?!";
							}
						} else {
							throw openmittsu::exceptions::InternalErrorException() << "Could not execute media existance query for uuid \"" << uuid.toStdString() << "\" table 'media'. Query error: " << query.lastError().text().toStdString();
						}
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Could not insert media data into 'media'. Query error: " << queryErrorMessage.toStdString();
					}
				}
			}

			void ExternalMediaFileStorage::removeMediaItem(QString const& uuid, MediaFileType const& fileType) {
				QFile::remove(m_storagePath.filePath(buildFilename(uuid, fileType)));

				QSqlQuery queryMedia(m_database->getQueryObject());
				queryMedia.prepare(QStringLiteral("DELETE FROM `media` WHERE `uid` = :uuid AND `type` = :type;"));
				queryMedia.bindValue(QStringLiteral(":uuid"), QVariant(uuid));
				queryMedia.bindValue(QStringLiteral(":type"), QVariant(MediaFileTypeHelper::toInt(fileType)));
				if (!queryMedia.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not delete media data from table 'media'. Query error: " << queryMedia.lastError().text().toStdString();
				}
			}

			void ExternalMediaFileStorage::removeAllMediaItems(QString const& uuid) {
				QFile::remove(m_storagePath.filePath(buildFilename(uuid, MediaFileType::TYPE_STANDARD)));
				QFile::remove(m_storagePath.filePath(buildFilename(uuid, MediaFileType::TYPE_THUMBNAIL)));

				QSqlQuery queryMedia(m_database->getQueryObject());
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
				if (!m_database->transactionStart()) {
					LOGGER()->warn("ExternalMediaFileStorage: Could NOT start transaction!");
				}

				auto it = items.constBegin();
				auto const end = items.constEnd();
				for (; it != end; ++it) {
					insertMediaItem(it->getUuid(), it->getData(), MediaFileType::TYPE_STANDARD);
				}

				if (!m_database->transactionCommit()) {
					LOGGER()->info("ExternalMediaFileStorage: Could NOT commit transaction!");
				}
			}

			void ExternalMediaFileStorage::insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) {
				if (!m_database->transactionStart()) {
					LOGGER()->warn("ExternalMediaFileStorage: Could NOT start transaction!");
				}

				auto it = items.constBegin();
				auto const end = items.constEnd();
				for (; it != end; ++it) {
					insertMediaItem(it->getUuid(), it->getData(), MediaFileType::TYPE_STANDARD);
				}

				if (!m_database->transactionCommit()) {
					LOGGER()->info("ExternalMediaFileStorage: Could NOT commit transaction!");
				}
			}

		}
	}
}
