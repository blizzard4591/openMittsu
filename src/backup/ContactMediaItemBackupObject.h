#ifndef OPENMITTSU_BACKUP_CONTACTMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_CONTACTMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/UuidMediaItemBackupObject.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class ContactMediaItemBackupObject : public UuidMediaItemBackupObject {
		public:
			ContactMediaItemBackupObject(QByteArray const& data, QString const& uuid);
			virtual ~ContactMediaItemBackupObject();

			virtual MediaItemType getType() const override;

			static ContactMediaItemBackupObject fromFile(QDir const& path, QString const& filename);
			static QHash<QString, QString> getContactMediaFiles(QDir const& path);
		};

	}
}

#endif // OPENMITTSU_BACKUP_CONTACTMEDIAITEMBACKUPOBJECT_H_
