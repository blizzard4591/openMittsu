#ifndef OPENMITTSU_BACKUP_CONTACTTHUMBNAILMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_CONTACTTHUMBNAILMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/UuidMediaItemBackupObject.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class ContactThumbnailMediaItemBackupObject : public UuidMediaItemBackupObject {
		public:
			ContactThumbnailMediaItemBackupObject(QByteArray const& data, QString const& uuid);
			virtual ~ContactThumbnailMediaItemBackupObject();

			virtual MediaItemType getType() const override;

			static ContactThumbnailMediaItemBackupObject fromFile(QDir const& path, QString const& filename);
			static QHash<QString, QString> getContactThumbnailMediaFiles(QDir const& path);
		};

	}
}

#endif // OPENMITTSU_BACKUP_CONTACTTHUMBNAILMEDIAITEMBACKUPOBJECT_H_
