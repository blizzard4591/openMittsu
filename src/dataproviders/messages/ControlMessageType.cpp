#include "src/dataproviders/messages/ControlMessageType.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			QString ControlMessageTypeHelper::toString(ControlMessageType const& messageType) {
				switch (messageType) {
					case ControlMessageType::RECEIVED:
						return QStringLiteral("RECEIVED");
					case ControlMessageType::READ:
						return QStringLiteral("READ");
					case ControlMessageType::AGREE:
						return QStringLiteral("AGREE");
					case ControlMessageType::DISAGREE:
						return QStringLiteral("DISAGREE");
					case ControlMessageType::GROUP_SYNC_REQUEST:
						return QStringLiteral("GROUP_SYNC_REQUEST");
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled ControlMessageType, this should never happen: " << static_cast<int>(messageType);
				}
			}

			ControlMessageType ControlMessageTypeHelper::fromString(QString const& messageTypeString) {
				if (messageTypeString == QStringLiteral("RECEIVED")) {
					return ControlMessageType::RECEIVED;
				} else if (messageTypeString == QStringLiteral("READ")) {
					return ControlMessageType::READ;
				} else if (messageTypeString == QStringLiteral("AGREE")) {
					return ControlMessageType::AGREE;
				} else if (messageTypeString == QStringLiteral("DISAGREE")) {
					return ControlMessageType::DISAGREE;
				} else if (messageTypeString == QStringLiteral("GROUP_SYNC_REQUEST")) {
					return ControlMessageType::GROUP_SYNC_REQUEST;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled ControlMessageType string representation, this should never happen: " << messageTypeString.toStdString();
				}
			}

		}
	}
}

