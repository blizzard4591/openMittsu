#include "src/dataproviders/messages/GroupMessageType.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			QString GroupMessageTypeHelper::toQString(GroupMessageType const& messageType) {
				switch (messageType) {
					case GroupMessageType::TEXT:
						return QStringLiteral("TEXT");
					case GroupMessageType::IMAGE:
						return QStringLiteral("IMAGE");
					case GroupMessageType::LOCATION:
						return QStringLiteral("LOCATION");
					case GroupMessageType::AUDIO:
						return QStringLiteral("AUDIO");
					case GroupMessageType::VIDEO:
						return QStringLiteral("VIDEO");
					case GroupMessageType::POLL:
						return QStringLiteral("POLL");
					case GroupMessageType::BALLOT:
						return QStringLiteral("BALLOT");
					case GroupMessageType::FILE:
						return QStringLiteral("FILE");
					case GroupMessageType::SYNC_REQUEST:
						return QStringLiteral("SYNC_REQUEST");
					case GroupMessageType::SET_TITLE:
						return QStringLiteral("SET_TITLE");
					case GroupMessageType::SET_IMAGE:
						return QStringLiteral("SET_IMAGE");
					case GroupMessageType::GROUP_CREATION:
						return QStringLiteral("GROUP_CREATION");
					case GroupMessageType::LEAVE:
						return QStringLiteral("LEAVE");
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled GroupMessageType, this should never happen: " << static_cast<int>(messageType);
				}
			}

			std::string GroupMessageTypeHelper::toString(GroupMessageType const& messageType) {
				return toQString(messageType).toStdString();
			}

			GroupMessageType GroupMessageTypeHelper::fromString(QString const& messageTypeString) {
				if (messageTypeString == QStringLiteral("TEXT")) {
					return GroupMessageType::TEXT;
				} else if (messageTypeString == QStringLiteral("IMAGE")) {
					return GroupMessageType::IMAGE;
				} else if (messageTypeString == QStringLiteral("LOCATION")) {
					return GroupMessageType::LOCATION;
				} else if (messageTypeString == QStringLiteral("AUDIO")) {
					return GroupMessageType::AUDIO;
				} else if (messageTypeString == QStringLiteral("VIDEO")) {
					return GroupMessageType::VIDEO;
				} else if (messageTypeString == QStringLiteral("POLL")) {
					return GroupMessageType::POLL;
				} else if (messageTypeString == QStringLiteral("BALLOT")) {
					return GroupMessageType::BALLOT;
				} else if (messageTypeString == QStringLiteral("FILE")) {
					return GroupMessageType::FILE;
				} else if (messageTypeString == QStringLiteral("SYNC_REQUEST")) {
					return GroupMessageType::SYNC_REQUEST;
				} else if (messageTypeString == QStringLiteral("SET_TITLE")) {
					return GroupMessageType::SET_TITLE;
				} else if (messageTypeString == QStringLiteral("SET_IMAGE")) {
					return GroupMessageType::SET_IMAGE;
				} else if (messageTypeString == QStringLiteral("GROUP_CREATION")) {
					return GroupMessageType::GROUP_CREATION;
				} else if (messageTypeString == QStringLiteral("LEAVE")) {
					return GroupMessageType::LEAVE;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled GroupMessageType string representation, this should never happen: " << messageTypeString.toStdString();
				}
			}

		}
	}
}
