#ifndef OPENMITTSU_BACKUP_UUIDMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_UUIDMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/MediaItemBackupObject.h"

#include <QString>

namespace openmittsu {
	namespace backup {

		class UuidMediaItemBackupObject : public MediaItemBackupObject {
		public:
			UuidMediaItemBackupObject(QByteArray const& data, QString const& uuid);
			virtual ~UuidMediaItemBackupObject();

			virtual MediaItemType getType() const = 0;

			QString const& getUuid() const;
		private:
			QString const m_uuid;
		};

	}
}

#endif // OPENMITTSU_BACKUP_UUIDMEDIAITEMBACKUPOBJECT_H_
