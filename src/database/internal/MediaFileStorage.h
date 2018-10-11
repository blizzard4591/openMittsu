#ifndef OPENMITTSU_DATABASE_INTERNAL_MEDIAFILESTORAGE_H_
#define OPENMITTSU_DATABASE_INTERNAL_MEDIAFILESTORAGE_H_

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QList>
#include <QString>

#include "src/database/MediaFileItem.h"

namespace openmittsu {
	namespace backup {
		class ContactMediaItemBackupObject;
		class GroupMediaItemBackupObject;
	}

	namespace database {
		namespace internal {

			class MediaFileStorage {
			public:
				virtual ~MediaFileStorage() {}

				virtual bool hasMediaItem(QString const& uuid) const = 0;
				virtual int getMediaItemCount() const = 0;

				virtual MediaFileItem getMediaItem(QString const& uuid) const = 0;
				virtual QString insertMediaItem(QByteArray const& data) = 0;
				virtual void removeMediaItem(QString const& uuid) = 0;

				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) = 0;
				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASEUSERMESSAGE_H_
