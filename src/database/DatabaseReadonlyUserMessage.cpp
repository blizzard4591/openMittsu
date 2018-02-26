#include "src/database/DatabaseReadonlyUserMessage.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {

		DatabaseReadonlyUserMessage::DatabaseReadonlyUserMessage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption) 
			: DatabaseReadonlyMessage(sender, messageId, isMessageFromUs, isOutbox, createdAt, sentAt, modifiedAt, isQueued, isSent, uuid), ReadonlyUserMessage(), m_isRead(isRead), m_isSaved(isSaved), m_messageState(messageState), m_receivedAt(receivedAt), m_seenAt(seenAt), m_isStatusMessage(isStatusMessage), m_caption(caption)
		{
			//
		}

		DatabaseReadonlyUserMessage::~DatabaseReadonlyUserMessage() {
			//
		}

		bool DatabaseReadonlyUserMessage::isRead() const {
			return m_isRead;
		}

		bool DatabaseReadonlyUserMessage::isSaved() const {
			return m_isSaved;
		}

		openmittsu::protocol::MessageTime DatabaseReadonlyUserMessage::getReceivedAt() const {
			return m_receivedAt;
		}

		openmittsu::protocol::MessageTime DatabaseReadonlyUserMessage::getSeenAt() const {
			return m_seenAt;
		}

		bool DatabaseReadonlyUserMessage::isStatusMessage() const {
			return m_isStatusMessage;
		}

		QString DatabaseReadonlyUserMessage::getCaption() const {
			return m_caption;
		}

		openmittsu::dataproviders::messages::UserMessageState DatabaseReadonlyUserMessage::getMessageState() const {
			return m_messageState;
		}

	}
}
