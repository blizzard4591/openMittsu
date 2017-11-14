#include "src/backup/BackupObject.h"

#include "src/utility/Logging.h"
#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace backup {
		
		BackupObject::BackupObject() {
			//
		}

		BackupObject::~BackupObject() {
			//
		}

		bool BackupObject::hasRequiredFields(QSet<QString> const& requiredFields, QHash<QString, int> const& headerOffsets) {
			auto it = requiredFields.constBegin();
			auto end = requiredFields.constEnd();
			for (; it != end; ++it) {
				QString const key = *it;
				if (!headerOffsets.contains(key)) {
					auto keys = headerOffsets.keys();
					QString keyString;
					for (int i = 0; i < keys.size(); ++i) {
						keyString = keyString.append(keys.at(i));
						if (i < (keys.size() - 1)) {
							keyString = keyString.append(',');
						}
					}

					LOGGER()->warn("Expected field {}, but header only contains columns {}.", key.toStdString(), keyString.toStdString());
					return false;
				}
			}

			return true;
		}

		void BackupObject::checkAndOpenFile(QFile& file) {
			if (!file.exists()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse file \"" << file.fileName().toStdString() << "\", it is not readable.";
			}

			if (!file.open(QIODevice::ReadOnly)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse file \"" << file.fileName().toStdString() << "\", it is not readable.";
			}
		}

	}
}