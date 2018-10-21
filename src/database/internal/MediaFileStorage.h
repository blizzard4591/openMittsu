#ifndef OPENMITTSU_DATABASE_INTERNAL_MEDIAFILESTORAGE_H_
#define OPENMITTSU_DATABASE_INTERNAL_MEDIAFILESTORAGE_H_

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QList>
#include <QString>

#include "src/database/MediaFileItem.h"
#include "src/database/MediaFileType.h"

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

				virtual bool hasMediaItem(QString const& uuid, MediaFileType const& fileType) const = 0;
				virtual int getMediaItemCount() const = 0;

				virtual MediaFileItem getMediaItem(QString const& uuid, MediaFileType const& fileType) const = 0;
				virtual void insertMediaItem(QString const& uuid, QByteArray const& data, MediaFileType const& fileType) = 0;
				virtual void removeMediaItem(QString const& uuid, MediaFileType const& fileType) = 0;

				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) = 0;
				virtual void insertMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) = 0;

				virtual void upgradeMediaDatabase(int fromVersion) = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASEUSERMESSAGE_H_
