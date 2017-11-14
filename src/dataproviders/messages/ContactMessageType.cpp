#include "src/dataproviders/messages/ContactMessageType.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			QString ContactMessageTypeHelper::toQString(ContactMessageType const& messageType) {
				switch (messageType) {
					case ContactMessageType::TEXT:
						return QStringLiteral("TEXT");
					case ContactMessageType::IMAGE:
						return QStringLiteral("IMAGE");
					case ContactMessageType::LOCATION:
						return QStringLiteral("LOCATION");
					case ContactMessageType::AUDIO:
						return QStringLiteral("AUDIO");
					case ContactMessageType::VIDEO:
						return QStringLiteral("VIDEO");
					case ContactMessageType::POLL:
						return QStringLiteral("POLL");
					case ContactMessageType::BALLOT:
						return QStringLiteral("BALLOT");
					case ContactMessageType::FILE:
						return QStringLiteral("FILE");
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled ContactMessageType, this should never happen: " << static_cast<int>(messageType);
				}
			}

			std::string ContactMessageTypeHelper::toString(ContactMessageType const& messageType) {
				return toQString(messageType).toStdString();
			}

			ContactMessageType ContactMessageTypeHelper::fromString(QString const& messageTypeString) {
				if (messageTypeString == QStringLiteral("TEXT")) {
					return ContactMessageType::TEXT;
				} else if (messageTypeString == QStringLiteral("IMAGE")) {
					return ContactMessageType::IMAGE;
				} else if (messageTypeString == QStringLiteral("LOCATION")) {
					return ContactMessageType::LOCATION;
				} else if (messageTypeString == QStringLiteral("AUDIO")) {
					return ContactMessageType::AUDIO;
				} else if (messageTypeString == QStringLiteral("VIDEO")) {
					return ContactMessageType::VIDEO;
				} else if (messageTypeString == QStringLiteral("POLL")) {
					return ContactMessageType::POLL;
				} else if (messageTypeString == QStringLiteral("BALLOT")) {
					return ContactMessageType::BALLOT;
				} else if (messageTypeString == QStringLiteral("FILE")) {
					return ContactMessageType::FILE;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled ContactMessageType string representation, this should never happen: " << messageTypeString.toStdString();
				}
			}

		}
	}
}
