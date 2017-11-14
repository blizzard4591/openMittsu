#include "src/backup/ContactThumbnailMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		ContactThumbnailMediaItemBackupObject::ContactThumbnailMediaItemBackupObject(QByteArray const& data, QString const& uuid) : UuidMediaItemBackupObject(data, uuid) {
			//
		}
		
		ContactThumbnailMediaItemBackupObject::~ContactThumbnailMediaItemBackupObject() {
			//
		}

		MediaItemBackupObject::MediaItemType ContactThumbnailMediaItemBackupObject::getType() const {
			return MediaItemBackupObject::MediaItemType::CONTACT_THUMBNAIL;
		}

		ContactThumbnailMediaItemBackupObject ContactThumbnailMediaItemBackupObject::fromFile(QDir const& path, QString const& filename) {
			if ((!filename.startsWith(QStringLiteral("message_thumbnail_"))) || (filename.size() != 54)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The file " << filename.toStdString() << " does not look like a contact thumbnail media item.";
			}
			QString const uuid = filename.right(36); // Length of the UUID
			
			QFile file(path.filePath(filename));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			return ContactThumbnailMediaItemBackupObject(data, uuid);
		}

		QHash<QString, QString> ContactThumbnailMediaItemBackupObject::getContactThumbnailMediaFiles(QDir const& path) {
			QHash<QString, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if ((filename.startsWith(QStringLiteral("message_thumbnail_"))) && (filename.size() == 54)) {
					QString const uuid = filename.right(36); // Length of the UUID
					result.insert(uuid, filename);
				}
			}

			return result;
		}

	}
}
