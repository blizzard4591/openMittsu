#include "src/backup/ContactMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"
#include <QFile>

namespace openmittsu {
	namespace backup {

		ContactMediaItemBackupObject::ContactMediaItemBackupObject(QByteArray const& data, QString const& uuid) : UuidMediaItemBackupObject(data, uuid) {
			//
		}
		
		ContactMediaItemBackupObject::~ContactMediaItemBackupObject() {
			//
		}

		MediaItemBackupObject::MediaItemType ContactMediaItemBackupObject::getType() const {
			return MediaItemBackupObject::MediaItemType::CONTACT_MEDIA;
		}

		ContactMediaItemBackupObject ContactMediaItemBackupObject::fromFile(QDir const& path, QString const& filename) {
			if ((!filename.startsWith(QStringLiteral("message_media_"))) || (filename.size() != 50)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The file " << filename.toStdString() << " does not look like a contact media item.";
			}
			QString const uuid = filename.right(36); // Length of the UUID
			
			QFile file(path.filePath(filename));
			checkAndOpenFile(file);
			QByteArray const data = file.readAll();
			file.close();

			return ContactMediaItemBackupObject(data, uuid);
		}

		QHash<QString, QString> ContactMediaItemBackupObject::getContactMediaFiles(QDir const& path) {
			QHash<QString, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if ((filename.startsWith(QStringLiteral("message_media_"))) && (filename.size() == 50)) {
					QString const uuid = filename.right(36); // Length of the UUID
					result.insert(uuid, filename);
				}
			}

			return result;
		}

	}
}
