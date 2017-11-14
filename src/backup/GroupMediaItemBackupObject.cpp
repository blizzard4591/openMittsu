#include "src/backup/GroupMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		GroupMediaItemBackupObject::GroupMediaItemBackupObject(QByteArray const& data, QString const& uuid) : UuidMediaItemBackupObject(data, uuid) {
			//
		}
		
		GroupMediaItemBackupObject::~GroupMediaItemBackupObject() {
			//
		}

		MediaItemBackupObject::MediaItemType GroupMediaItemBackupObject::getType() const {
			return MediaItemBackupObject::MediaItemType::GROUP_MEDIA;
		}

		GroupMediaItemBackupObject GroupMediaItemBackupObject::fromFile(QDir const& path, QString const& filename) {
			if ((!filename.startsWith(QStringLiteral("group_message_media_"))) || (filename.size() != 56)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The file " << filename.toStdString() << " does not look like a group media item.";
			}
			QString const uuid = filename.right(36); // Length of the UUID
			
			QFile file(path.filePath(filename));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			return GroupMediaItemBackupObject(data, uuid);
		}

		QHash<QString, QString> GroupMediaItemBackupObject::getGroupMediaFiles(QDir const& path) {
			QHash<QString, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if ((filename.startsWith(QStringLiteral("group_message_media_"))) && (filename.size() == 56)) {
					QString const uuid = filename.right(36); // Length of the UUID
					result.insert(uuid, filename);
				}
			}

			return result;
		}

	}
}
