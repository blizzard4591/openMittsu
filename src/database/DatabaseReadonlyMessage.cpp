#include "src/database/DatabaseReadonlyMessage.h"

#include "src/database/internal/DatabaseUtilities.h"
#include "src/database/MediaFileItem.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseReadonlyMessage::DatabaseReadonlyMessage(openmittsu::protocol::ContactId sender, openmittsu::protocol::MessageId messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime createdAt, openmittsu::protocol::MessageTime sentAt, openmittsu::protocol::MessageTime modifiedAt, bool isQueued, bool isSent, QString uuid) : 
		m_sender(sender), m_messageId(messageId), m_isMessageFromUs(isMessageFromUs), m_isOutbox(isOutbox), m_createdAt(createdAt), m_sentAt(sentAt), m_modifiedAt(modifiedAt), m_isQueued(isQueued), m_isSent(isSent), m_uuid(uuid) {
			//
		}
			
		DatabaseReadonlyMessage::~DatabaseReadonlyMessage() {
			//
		}

		openmittsu::protocol::MessageId const& DatabaseReadonlyMessage::getMessageId() const {
			return m_messageId;
		}

		bool DatabaseReadonlyMessage::isOutbox() const {
			return m_isOutbox;
		}

		openmittsu::protocol::MessageTime DatabaseReadonlyMessage::getCreatedAt() const {
			return m_createdAt;
		}

		openmittsu::protocol::MessageTime DatabaseReadonlyMessage::getSentAt() const {
			return m_sentAt;
		}

		openmittsu::protocol::MessageTime DatabaseReadonlyMessage::getModifiedAt() const {
			return m_modifiedAt;
		}

		bool DatabaseReadonlyMessage::isQueued() const {
			return m_isQueued;
		}

		bool DatabaseReadonlyMessage::isSent() const {
			return m_isSent;
		}

		QString DatabaseReadonlyMessage::getUid() const {
			return m_uuid;
		}

		openmittsu::protocol::ContactId DatabaseReadonlyMessage::getSender() const {
			return m_sender;
		}

		bool DatabaseReadonlyMessage::isMessageFromUs() const {
			return m_isMessageFromUs;
		}

	}
}
