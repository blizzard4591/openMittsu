#include "src/backup/UuidMediaItemBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace backup {

		UuidMediaItemBackupObject::UuidMediaItemBackupObject(QByteArray const& data, QString const& uuid) : MediaItemBackupObject(data), m_uuid(uuid) {
			//
		}
		
		UuidMediaItemBackupObject::~UuidMediaItemBackupObject() {
			//
		}

		QString const& UuidMediaItemBackupObject::getUuid() const {
			return m_uuid;
		}
	}
}
