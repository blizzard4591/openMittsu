#ifndef OPENMITTSU_BACKUP_GROUPMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_GROUPMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/UuidMediaItemBackupObject.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class GroupMediaItemBackupObject : public UuidMediaItemBackupObject {
		public:
			GroupMediaItemBackupObject(QByteArray const& data, QString const& uuid);
			virtual ~GroupMediaItemBackupObject();

			virtual MediaItemType getType() const override;

			static GroupMediaItemBackupObject fromFile(QDir const& path, QString const& filename);
			static QHash<QString, QString> getGroupMediaFiles(QDir const& path);
		};

	}
}

#endif // OPENMITTSU_BACKUP_GROUPMEDIAITEMBACKUPOBJECT_H_
