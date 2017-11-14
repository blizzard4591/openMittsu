#include "src/backup/MediaItemBackupObject.h"

namespace openmittsu {
	namespace backup {

		MediaItemBackupObject::MediaItemBackupObject(QByteArray const& data) : m_data(data) {
			//
		}
		
		MediaItemBackupObject::~MediaItemBackupObject() {
			//
		}

		QByteArray const& MediaItemBackupObject::getData() const {
			return m_data;
		}
	}
}
