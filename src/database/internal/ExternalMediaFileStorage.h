#ifndef OPENMITTSU_DATABASE_INTERNAL_EXTERNALMEDIAFILESTORAGE_H_
#define OPENMITTSU_DATABASE_INTERNAL_EXTERNALMEDIAFILESTORAGE_H_

#include "src/database/internal/MediaFileStorage.h"
#include <utility>

namespace openmittsu {
	namespace database {
		namespace internal {
			class InternalDatabaseInterface;

			class ExternalMediaFileStorage : public MediaFileStorage {
			public:
				explicit ExternalMediaFileStorage(QDir const& storagePath, InternalDatabaseInterface* database);
				virtual ~ExternalMediaFileStorage();

				virtual bool hasMediaItem(QString const& uuid, MediaFileType const& fileType) const override;
				virtual int getMediaItemCount() const override;

				virtual MediaFileItem getMediaItem(QString const& uuid, MediaFileType const& fileType) const override;
				virtual void insertMediaItem(QString const& uuid, QByteArray const& data, MediaFileType const& fileType) override;
				virtual void removeMediaItem(QString const& uuid, MediaFileType const& fileType) override;
				virtual void removeAllMediaItems(QString const& uuid) override;

				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) override;
				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) override;

				virtual void upgradeMediaDatabase(int fromVersion) override;
			private:
				QString buildFilename(QString const& uuid, MediaFileType const& fileType) const;

				int cryptoGetNonceSize() const;
				int cryptoGetHeaderSize() const;
				int cryptoGetKeySize() const;

				QByteArray decrypt(QByteArray const& encryptedData, QByteArray const& key, QByteArray const& nonce) const;
				QByteArray generateKey() const;
				QByteArray generateNonce() const;
				QByteArray encrypt(QByteArray const& unencryptedData, QByteArray const& key, QByteArray const& nonce) const;

				QDir const m_storagePath;
				InternalDatabaseInterface* const m_database;
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_EXTERNALMEDIAFILESTORAGE_H_
