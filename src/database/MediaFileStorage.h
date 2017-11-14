#ifndef OPENMITTSU_DATABASE_MEDIAFILESTORAGE_H_
#define OPENMITTSU_DATABASE_MEDIAFILESTORAGE_H_

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QList>
#include <QString>

namespace openmittsu {
	namespace backup {
		class ContactMediaItemBackupObject;
		class GroupMediaItemBackupObject;
	}

	namespace database {
		
		class MediaFileStorage {
		public:
			virtual ~MediaFileStorage() {}

			virtual bool hasMediaItem(QString const& uuid) const = 0;
			virtual int getMediaItemCount() const = 0;

			virtual QByteArray getMediaItem(QString const& uuid) const = 0;
			virtual QString insertMediaItem(QByteArray const& data) = 0;
			virtual void removeMediaItem(QString const& uuid) = 0;

			virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) = 0;
			virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) = 0;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEUSERMESSAGE_H_
