#include "src/backup/GroupAvatarMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		GroupAvatarMediaItemBackupObject::GroupAvatarMediaItemBackupObject(QByteArray const& data, openmittsu::protocol::GroupId const& groupId) : MediaItemBackupObject(data), m_groupId(groupId) {
			//
		}
		
		GroupAvatarMediaItemBackupObject::~GroupAvatarMediaItemBackupObject() {
			//
		}

		MediaItemBackupObject::MediaItemType GroupAvatarMediaItemBackupObject::getType() const {
			return MediaItemBackupObject::MediaItemType::GROUP_AVATAR;
		}

		GroupAvatarMediaItemBackupObject GroupAvatarMediaItemBackupObject::fromFile(QDir const& path, QString const& filename) {
			if ((!filename.startsWith(QStringLiteral("group_avatar_"))) || (filename.size() != 38)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The file " << filename.toStdString() << " does not look like a group avatar item.";
			}
			
			QString const groupIdOwnerString = filename.right(8);
			QString const groupIdIdString = filename.mid(13, 16);
			openmittsu::protocol::ContactId const groupOwner(groupIdOwnerString);
			openmittsu::protocol::GroupId const groupId(groupOwner, groupIdIdString);
			
			QFile file(path.filePath(filename));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			return GroupAvatarMediaItemBackupObject(data, groupId);
		}

		QHash<openmittsu::protocol::GroupId, QString> GroupAvatarMediaItemBackupObject::getGroupAvatarMediaFiles(QDir const& path) {
			QHash<openmittsu::protocol::GroupId, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if ((filename.startsWith(QStringLiteral("group_avatar_"))) && (filename.size() == 38)) {
					QString const groupIdOwnerString = filename.right(8);
					QString const groupIdIdString = filename.mid(13, 16);
					openmittsu::protocol::ContactId const groupOwner(groupIdOwnerString);
					openmittsu::protocol::GroupId const groupId(groupOwner, groupIdIdString);
					result.insert(groupId, filename);
				}
			}

			return result;
		}

	}
}
