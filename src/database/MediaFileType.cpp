#include "src/database/MediaFileType.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace database {

		int MediaFileTypeHelper::toInt(MediaFileType const& messageType) {
			switch (messageType) {
				case MediaFileType::TYPE_STANDARD:
					return 1;
				case MediaFileType::TYPE_THUMBNAIL:
					return 2;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled MediaFileType, this should never happen: " << static_cast<int>(messageType);
			}
		}

		QString MediaFileTypeHelper::toQString(MediaFileType const& messageType) {
			switch (messageType) {
				case MediaFileType::TYPE_STANDARD:
					return QStringLiteral("STANDARD");
				case MediaFileType::TYPE_THUMBNAIL:
					return QStringLiteral("THUMBNAIL");
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled MediaFileType, this should never happen: " << static_cast<int>(messageType);
			}
		}

		std::string MediaFileTypeHelper::toString(MediaFileType const& messageType) {
			return toQString(messageType).toStdString();
		}

		MediaFileType MediaFileTypeHelper::fromString(QString const& messageTypeString) {
			if (messageTypeString == QStringLiteral("STANDARD")) {
				return MediaFileType::TYPE_STANDARD;
			} else if (messageTypeString == QStringLiteral("THUMBNAIL")) {
				return MediaFileType::TYPE_THUMBNAIL;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unhandled MediaFileType string representation, this should never happen: " << messageTypeString.toStdString();
			}
		}

		MediaFileType MediaFileTypeHelper::fromInt(int messageTypeInt) {
			if (messageTypeInt == 1) {
				return MediaFileType::TYPE_STANDARD;
			} else if (messageTypeInt == 2) {
				return MediaFileType::TYPE_THUMBNAIL;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unhandled MediaFileType string representation, this should never happen: " << messageTypeInt;
			}
		}

	}
}
