#include "src/backup/GroupThumbnailMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		GroupThumbnailMediaItemBackupObject::GroupThumbnailMediaItemBackupObject(QByteArray const& data, QString const& uuid) : UuidMediaItemBackupObject(data, uuid) {
			//
		}
		
		GroupThumbnailMediaItemBackupObject::~GroupThumbnailMediaItemBackupObject() {
			//
		}

		MediaItemBackupObject::MediaItemType GroupThumbnailMediaItemBackupObject::getType() const {
			return MediaItemBackupObject::MediaItemType::GROUP_THUMBNAIL;
		}

		GroupThumbnailMediaItemBackupObject GroupThumbnailMediaItemBackupObject::fromFile(QDir const& path, QString const& filename) {
			if ((!filename.startsWith(QStringLiteral("group_message_thumbnail_"))) || (filename.size() != 60)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The file " << filename.toStdString() << " does not look like a group thumbnail media item.";
			}
			QString const uuid = filename.right(36); // Length of the UUID
			
			QFile file(path.filePath(filename));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			return GroupThumbnailMediaItemBackupObject(data, uuid);
		}

		QHash<QString, QString> GroupThumbnailMediaItemBackupObject::getGroupThumbnailMediaFiles(QDir const& path) {
			QHash<QString, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if ((filename.startsWith(QStringLiteral("group_message_thumbnail_"))) && (filename.size() == 60)) {
					QString const uuid = filename.right(36); // Length of the UUID
					result.insert(uuid, filename);
				}
			}

			return result;
		}

	}
}
