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

		DatabaseReadonlyContactMessage::DatabaseReadonlyContactMessage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption, openmittsu::dataproviders::messages::ContactMessageType const& contactMessageType, QString const& body, MediaFileItem const& mediaItem)
			: ReadonlyContactMessage(), m_sender(sender), m_messageId(messageId), m_isMessageFromUs(isMessageFromUs), m_createdAt(createdAt), m_sentAt(sentAt), m_modifiedAt(modifiedAt), m_isQueued(isQueued), m_isSent(isSent), m_uuid(uuid), m_isRead(isRead), m_isSaved(isSaved), m_messageState(messageState), m_receivedAt(receivedAt), m_seenAt(seenAt), m_isStatusMessage(isStatusMessage), m_caption(caption), m_contactMessageType(contactMessageType), m_body(body), m_mediaItem(mediaItem)
		{
			//
		}

		DatabaseReadonlyContactMessage::~DatabaseReadonlyContactMessage() {
			//
		}

		openmittsu::protocol::ContactId const& DatabaseReadonlyContactMessage::getContactId() const {
			return m_sender;
		}

		ContactMessageType const& DatabaseReadonlyContactMessage::getMessageType() const {
			return m_contactMessageType;
		}

		openmittsu::protocol::ContactId const& DatabaseReadonlyContactMessage::getSender() const {
			return m_sender;
		}

		openmittsu::protocol::MessageId const& DatabaseReadonlyContactMessage::getMessageId() const {
			return m_messageId;
		}

		bool DatabaseReadonlyContactMessage::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyContactMessage::getCreatedAt() const {
			return m_createdAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyContactMessage::getSentAt() const {
			return m_sentAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyContactMessage::getModifiedAt() const {
			return m_modifiedAt;
		}

		/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
		bool DatabaseReadonlyContactMessage::isQueued() const {
			return m_isQueued;
		}

		/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
		bool DatabaseReadonlyContactMessage::isSent() const {
			return m_isSent;
		}

		QString const& DatabaseReadonlyContactMessage::getUid() const {
			return m_uuid;
		}

		bool DatabaseReadonlyContactMessage::isRead() const {
			return m_isRead;
		}

		/** Encodes whether all data is available, i.e. referenced image or video data is available. */
		bool DatabaseReadonlyContactMessage::isSaved() const {
			return m_isSaved;
		}

		openmittsu::dataproviders::messages::UserMessageState const& DatabaseReadonlyContactMessage::getMessageState() const {
			return m_messageState;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyContactMessage::getReceivedAt() const {
			return m_receivedAt;
		}

		openmittsu::protocol::MessageTime const& DatabaseReadonlyContactMessage::getSeenAt() const {
			return m_seenAt;
		}

		bool DatabaseReadonlyContactMessage::isStatusMessage() const {
			return m_isStatusMessage;
		}

		QString const& DatabaseReadonlyContactMessage::getCaption() const {
			return m_caption;
		}

		QString const& DatabaseReadonlyContactMessage::getContentAsText() const {
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