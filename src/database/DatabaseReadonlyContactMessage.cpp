#include "src/database/DatabaseReadonlyContactMessage.h"

#include "src/backup/ContactMessageBackupObject.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>
#include <chrono>

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseReadonlyContactMessage::DatabaseReadonlyContactMessage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption, openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::messages::ContactMessageType const& contactMessageType, QString const& body, MediaFileItem const& mediaItem)
			: DatabaseReadonlyUserMessage(sender, messageId, isMessageFromUs, isOutbox, createdAt, sentAt, modifiedAt, isQueued, isSent, uuid, isRead, isSaved, messageState, receivedAt, seenAt, isStatusMessage, caption), DatabaseReadonlyMessage(sender, messageId, isMessageFromUs, isOutbox, createdAt, sentAt, modifiedAt, isQueued, isSent, uuid), ReadonlyContactMessage(), m_contact(contact), m_contactMessageType(contactMessageType), m_body(body), m_mediaItem(mediaItem)
		{
			//
		}

		DatabaseReadonlyContactMessage::~DatabaseReadonlyContactMessage() {
			//
		}

		openmittsu::protocol::ContactId const& DatabaseReadonlyContactMessage::getContactId() const {
			return m_contact;
		}

		ContactMessageType DatabaseReadonlyContactMessage::getMessageType() const {
			return m_contactMessageType;
		}

		QString DatabaseReadonlyContactMessage::getContentAsText() const {
			ContactMessageType const messageType = getMessageType();
			if (messageType != ContactMessageType::TEXT) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as text because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
			}
			return m_body;
		}

		openmittsu::utility::Location DatabaseReadonlyContactMessage::getContentAsLocation() const {
			ContactMessageType const messageType = getMessageType();
			if (messageType != ContactMessageType::LOCATION) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as location because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
			}
			return openmittsu::utility::Location::fromDatabaseString(m_body);
		}

		MediaFileItem DatabaseReadonlyContactMessage::getContentAsImage() const {
			ContactMessageType const messageType = getMessageType();
			if (messageType != ContactMessageType::IMAGE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as image because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
			}
			return m_mediaItem;
		}

	}
}