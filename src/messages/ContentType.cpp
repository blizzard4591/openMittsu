#include "src/messages/ContentType.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace messages {

		QString ContentTypeHelper::convertContentTypeToString(ContentType const& contentType) {
			switch (contentType) {
				case ContentType::TYPE_UNKNOWN:
					return QStringLiteral("TYPE_UNKNOWN");
					break;
				case ContentType::TYPE_CONTACT_TEXT:
					return QStringLiteral("TYPE_CONTACT_TEXT");
					break;
				case ContentType::TYPE_CONTACT_PICTURE:
					return QStringLiteral("TYPE_CONTACT_PICTURE");
					break;
				case ContentType::TYPE_RECEIPT:
					return QStringLiteral("TYPE_RECEIPT");
					break;
				case ContentType::TYPE_TYPINGNOTIFICATION:
					return QStringLiteral("TYPE_TYPINGNOTIFICATION");
					break;
				case ContentType::TYPE_GROUP_CREATION:
					return QStringLiteral("TYPE_GROUP_CREATION");
					break;
				case ContentType::TYPE_GROUP_TITLE:
					return QStringLiteral("TYPE_GROUP_TITLE");
					break;
				case ContentType::TYPE_GROUP_MESSAGE:
					return QStringLiteral("TYPE_GROUP_MESSAGE");
					break;
				case ContentType::TYPE_GROUP_LEAVE:
					return QStringLiteral("TYPE_GROUP_LEAVE");
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "ContentTypeHelper::convertContentTypeToString called with unknown ContentType.";
			}
		}

	}
}
