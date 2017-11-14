#include "src/dataproviders/BackedGroupMessage.h"

#include "src/utility/QObjectConnectionMacro.h"

#include "MessageCenter.h"
#include "src/database/Database.h"

namespace openmittsu {
	namespace dataproviders {

		BackedGroupMessage::BackedGroupMessage(std::shared_ptr<messages::GroupMessage> const& message, BackedContact const& sender, openmittsu::dataproviders::MessageCenter& messageCenter) : BackedMessage(message->getUid(), sender, message->isMessageFromUs(), message->getMessageId()), m_message(message), m_messageCenter(messageCenter) {
			OPENMITTSU_CONNECT(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
			loadCache();
		}

		BackedGroupMessage::BackedGroupMessage(BackedGroupMessage const& other) : BackedMessage(other), m_message(other.m_message), m_messageCenter(other.m_messageCenter) {
			OPENMITTSU_CONNECT(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
			loadCache();
		}

		BackedGroupMessage::~BackedGroupMessage() {
			//
		}

		void BackedGroupMessage::setIsSeen() {
			m_messageCenter.sendReceipt(m_message->getGroupId(), m_message->getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN);
		}

		messages::UserMessage const& BackedGroupMessage::getMessage() const {
			return *m_message;
		}

		messages::GroupMessageType BackedGroupMessage::getMessageType() const {
			return m_message->getMessageType();
		}

	}
}
