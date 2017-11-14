#ifndef OPENMITTSU_BACKUP_GROUPTHUMBNAILMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_GROUPTHUMBNAILMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/UuidMediaItemBackupObject.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class GroupThumbnailMediaItemBackupObject : public UuidMediaItemBackupObject {
		public:
			GroupThumbnailMediaItemBackupObject(QByteArray const& data, QString const& uuid);
			virtual ~GroupThumbnailMediaItemBackupObject();

			virtual MediaItemType getType() const override;

			static GroupThumbnailMediaItemBackupObject fromFile(QDir const& path, QString const& filename);
			static QHash<QString, QString> getGroupThumbnailMediaFiles(QDir const& path);
		};

	}
}

#endif // OPENMITTSU_BACKUP_GROUPTHUMBNAILMEDIAITEMBACKUPOBJECT_H_
