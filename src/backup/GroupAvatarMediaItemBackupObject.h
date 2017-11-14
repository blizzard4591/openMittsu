#ifndef OPENMITTSU_BACKUP_GROUPAVATARMEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_GROUPAVATARMEDIAITEMBACKUPOBJECT_H_

#include "src/backup/MediaItemBackupObject.h"
#include "src/protocol/GroupId.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class GroupAvatarMediaItemBackupObject : public MediaItemBackupObject {
		public:
			GroupAvatarMediaItemBackupObject(QByteArray const& data, openmittsu::protocol::GroupId const& groupId);
			virtual ~GroupAvatarMediaItemBackupObject();

			virtual MediaItemType getType() const override;

			static GroupAvatarMediaItemBackupObject fromFile(QDir const& path, QString const& filename);
			static QHash<openmittsu::protocol::GroupId, QString> getGroupAvatarMediaFiles(QDir const& path);
		private:
			openmittsu::protocol::GroupId const m_groupId;
		};

	}
}

#endif // OPENMITTSU_BACKUP_GROUPAVATARMEDIAITEMBACKUPOBJECT_H_
