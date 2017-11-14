#ifndef OPENMITTSU_BACKUP_MEDIAITEMBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_MEDIAITEMBACKUPOBJECT_H_

#include "src/backup/BackupObject.h"
#include "src/crypto/Crc32.h"

#include <QByteArray>
#include <QDir>
#include <QString>

namespace openmittsu {
	namespace backup {

		class MediaItemBackupObject : public BackupObject {
		public:
			enum class MediaItemType {
				CONTACT_MEDIA, CONTACT_THUMBNAIL, GROUP_MEDIA, GROUP_THUMBNAIL, GROUP_AVATAR
			};

			MediaItemBackupObject(QByteArray const& data);
			virtual ~MediaItemBackupObject();

			virtual MediaItemType getType() const = 0;
			QByteArray const& getData() const;
		private:
			QByteArray m_data;
		};

	}
}

#endif // OPENMITTSU_BACKUP_MEDIAITEMBACKUPOBJECT_H_
