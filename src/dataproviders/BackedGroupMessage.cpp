#include "src/dataproviders/BackedGroupMessage.h"

#include "src/utility/QObjectConnectionMacro.h"

#include "MessageCenter.h"
#include "src/database/Database.h"

namespace openmittsu {
	namespace dataproviders {

		BackedGroupMessage::BackedGroupMessage(openmittsu::database::DatabaseReadonlyGroupMessage const& message, std::shared_ptr<BackedContact> const& sender, std::shared_ptr<BackedGroup> const& group, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) : BackedMessage(message.getUid(), sender, message.isMessageFromUs(), message.getMessageId()), m_group(group), m_message(message), m_messageCenter(messageCenter) {
			OPENMITTSU_CONNECT_QUEUED(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
		}

		BackedGroupMessage::BackedGroupMessage(BackedGroupMessage const& other) : BackedMessage(other), m_group(other.m_group), m_message(other.m_message), m_messageCenter(other.m_messageCenter) {
			OPENMITTSU_CONNECT_QUEUED(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
		}

		BackedGroupMessage::~BackedGroupMessage() {
			//
		}

		void BackedGroupMessage::setIsSeen() {
			m_messageCenter.sendReceipt(m_message.getGroupId(), m_message.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN);
		}

		messages::ReadonlyUserMessage const& BackedGroupMessage::getMessage() const {
			return m_message;
		}

		void BackedGroupMessage::loadCache() {
			m_message = m_group->fetchMessageByUuid(m_uuid);
		}

		messages::GroupMessageType BackedGroupMessage::getMessageType() const {
			return m_message.getMessageType();
		}

	}
}
