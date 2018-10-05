#include "src/dataproviders/BackedContactMessage.h"

#include "src/utility/QObjectConnectionMacro.h"

#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		BackedContactMessage::BackedContactMessage(openmittsu::database::DatabaseReadonlyContactMessage const& message, std::shared_ptr<BackedContact> const& sender, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) : BackedMessage(message.getUid(), sender, message.isMessageFromUs(), message.getMessageId()), m_message(message), m_messageCenter(messageCenter) {
			OPENMITTSU_CONNECT(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
		}

		BackedContactMessage::BackedContactMessage(BackedContactMessage const& other) : BackedMessage(other), m_message(other.m_message), m_messageCenter(other.m_messageCenter) {
			OPENMITTSU_CONNECT(&m_messageCenter, messageChanged(QString const&), this, onMessageChanged(QString const&));
		}

		BackedContactMessage::~BackedContactMessage() {
			//
		}

		void BackedContactMessage::setIsSeen() {
			m_messageCenter.sendReceipt(m_message.getContactId(), m_message.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN);
		}

		void BackedContactMessage::setIsAgreed() {
			m_messageCenter.sendReceipt(m_message.getContactId(), m_message.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE);
		}

		void BackedContactMessage::setIsDisagreed() {
			m_messageCenter.sendReceipt(m_message.getContactId(), m_message.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE);
		}

		messages::ReadonlyUserMessage const& BackedContactMessage::getMessage() const {
			return m_message;
		}

		void BackedContactMessage::loadCache() {
			m_message = m_contact->fetchMessageByUuid(m_uuid);
		}

		messages::ContactMessageType BackedContactMessage::getMessageType() const {
			return m_message.getMessageType();
		}
	}
}
