#ifndef OPENMITTSU_DATABASE_EXTERNALMEDIAFILESTORAGE_H_
#define OPENMITTSU_DATABASE_EXTERNALMEDIAFILESTORAGE_H_

#include "src/database/MediaFileStorage.h"
#include <utility>

namespace openmittsu {
	namespace database {
		class Database;

		class ExternalMediaFileStorage : public MediaFileStorage {
		public:
			explicit ExternalMediaFileStorage(QDir const& storagePath, openmittsu::database::Database& database);
			virtual ~ExternalMediaFileStorage();

			virtual bool hasMediaItem(QString const& uuid) const override;
			virtual int getMediaItemCount() const override;

			virtual MediaFileItem getMediaItem(QString const& uuid) const override;
			virtual QString insertMediaItem(QByteArray const& data) override;
			virtual void removeMediaItem(QString const& uuid) override;

			virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) override;
			virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) override;
		private:
			QString buildFilename(QString const& uuid) const;
			
			int cryptoGetNonceSize() const;
			int cryptoGetHeaderSize() const;
			int cryptoGetKeySize() const;

			void insertMediaItem(QString const& uuid, QByteArray const& data);
			QByteArray decrypt(QByteArray const& encryptedData, QByteArray const& key, QByteArray const& nonce) const;
			QByteArray generateKey() const;
			QByteArray generateNonce() const;
			QByteArray encrypt(QByteArray const& unencryptedData, QByteArray const& key, QByteArray const& nonce) const;

			QDir const m_storagePath;
			openmittsu::database::Database& m_database;
		};

	}
}

#endif // OPENMITTSU_DATABASE_EXTERNALMEDIAFILESTORAGE_H_
