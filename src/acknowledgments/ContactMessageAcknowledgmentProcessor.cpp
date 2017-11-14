#include "src/acknowledgments/ContactMessageAcknowledgmentProcessor.h"

#include "src/dataproviders/MessageCenter.h"
#include "src/network/ProtocolClient.h"
#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace acknowledgments {

		ContactMessageAcknowledgmentProcessor::ContactMessageAcknowledgmentProcessor(openmittsu::protocol::ContactId const& receiver, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& messageId) : AcknowledgmentProcessor(timeoutTime), m_receiver(receiver), m_messageId(messageId), m_receivedAck(false) {
			// Intentionally left empty.
		}

		ContactMessageAcknowledgmentProcessor::~ContactMessageAcknowledgmentProcessor() {
			// Intentionally left empty.
		}

		openmittsu::protocol::ContactId const& ContactMessageAcknowledgmentProcessor::getReceiver() const {
			return m_receiver;
		}

		openmittsu::protocol::MessageId const& ContactMessageAcknowledgmentProcessor::getMessageId() const {
			return m_messageId;
		}

		bool ContactMessageAcknowledgmentProcessor::isDone() const {
			return m_receivedAck;
		}

		void ContactMessageAcknowledgmentProcessor::sendFailedTimeout(openmittsu::network::ProtocolClient* protocolClient) {
			m_receivedAck = true;
			contactMessageSendFailed(m_receiver, m_messageId, protocolClient);
		}

		void ContactMessageAcknowledgmentProcessor::sendFailed(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) {
			if (m_messageId != messageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Received a SEND_FAILED for a message with ID #" << messageId.toString() << " on a ContactMessageAcknowledgmentProcessor built for Message ID #" << m_messageId.toString() << ".";
			}
			m_receivedAck = true;
			contactMessageSendFailed(m_receiver, m_messageId, protocolClient);
		}

		void ContactMessageAcknowledgmentProcessor::sendSuccess(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) {
			if (m_messageId != messageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Received a SEND_SUCCESS for a message with ID #" << messageId.toString() << " on a ContactMessageAcknowledgmentProcessor built for Message ID #" << m_messageId.toString() << ".";
			}
			m_receivedAck = true;
			contactMessageSendSuccess(m_receiver, m_messageId, protocolClient);
		}

		void ContactMessageAcknowledgmentProcessor::addMessage(openmittsu::protocol::MessageId const& addedMessageId) {
			if (m_messageId != addedMessageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Tried adding Message ID " << addedMessageId.toString() << " to ContactMessageAcknowledgmentProcessor built for Message ID " << m_messageId.toString() << ".";
			}
			m_receivedAck = false;
		}

	}
}
