#include "src/acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"

#include "src/network/ProtocolClient.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace acknowledgments {

		GroupContentMessageAcknowledgmentProcessor::GroupContentMessageAcknowledgmentProcessor(openmittsu::protocol::GroupId const& groupId, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& messageId) : AcknowledgmentProcessor(timeoutTime), m_messageCount(0u), m_hasFailedMessage(false), m_groupId(groupId), m_messageId(messageId) {
			// Intentionally left empty.
		}

		GroupContentMessageAcknowledgmentProcessor::~GroupContentMessageAcknowledgmentProcessor() {
			// Intentionally left empty.
		}

		openmittsu::protocol::GroupId const& GroupContentMessageAcknowledgmentProcessor::getGroupId() const {
			return m_groupId;
		}

		openmittsu::protocol::MessageId const& GroupContentMessageAcknowledgmentProcessor::getGroupUniqueMessageId() const {
			return m_messageId;
		}

		bool GroupContentMessageAcknowledgmentProcessor::isDone() const {
			return m_messageCount == 0;
		}

		void GroupContentMessageAcknowledgmentProcessor::addMessage(openmittsu::protocol::MessageId const& addedMessageId) {
			if (m_messageId != addedMessageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Tried adding Message ID #" << addedMessageId.toString() << " to GroupContentMessageAcknowledgmentProcessor that was built for a different message ID #" << m_messageId.toString() << ".";
			}
			QMutexLocker lock(&m_mutex);
			m_messageCount++;
			LOGGER_DEBUG("Added message with ID {} to GroupContentMessageAcknowledgmentProcessor for group {}, now have a total of {} messages.", addedMessageId.toString(), m_groupId.toString(), m_messageCount);
		}

		void GroupContentMessageAcknowledgmentProcessor::sendFailedTimeout(openmittsu::network::ProtocolClient* protocolClient) {
			QMutexLocker lock(&m_mutex);
			
			m_messageCount = 0;
			m_hasFailedMessage = true;
			sendResultIfDone(protocolClient);
		}

		void GroupContentMessageAcknowledgmentProcessor::sendFailed(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) {
			if (m_messageId != messageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Received a SEND_FAILED for a message with ID #" << messageId.toString() << " on a GroupContentMessageAcknowledgmentProcessor built for Message ID #" << m_messageId.toString() << ".";
			}
			
			QMutexLocker lock(&m_mutex);
			if (m_messageCount > 0) {
				m_hasFailedMessage = true;
				m_messageCount--;
				LOGGER_DEBUG("Send failed for message with ID {} in GroupContentMessageAcknowledgmentProcessor for group {}, now have a total of {} messages.", messageId.toString(), m_groupId.toString(), m_messageCount);
				sendResultIfDone(protocolClient);
			} else {
				LOGGER()->warn("GroupContentMessageAcknowledgmentProcessor received a MESSAGE_FAILED notification for group {}, message ID {}, that was not recognized.", m_groupId.toString(), m_messageId.toString());
			}
		}

		void GroupContentMessageAcknowledgmentProcessor::sendSuccess(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) {
			if (m_messageId != messageId) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Received a SEND_SUCCESS for a message with ID #" << messageId.toString() << " on a GroupContentMessageAcknowledgmentProcessor built for Message ID #" << m_messageId.toString() << ".";
			}

			QMutexLocker lock(&m_mutex);
			if (m_messageCount > 0) {
				m_messageCount--;
				LOGGER_DEBUG("Send success for message with ID {} in GroupContentMessageAcknowledgmentProcessor for group {}, now have a total of {} messages.", messageId.toString(), m_groupId.toString(), m_messageCount);
				sendResultIfDone(protocolClient);
			} else {
				LOGGER()->warn("GroupContentMessageAcknowledgmentProcessor received a MESSAGE_SUCCESS notification for group {}, message ID {}, that was not recognized.", m_groupId.toString(), m_messageId.toString());
			}
		}

		void GroupContentMessageAcknowledgmentProcessor::sendResultIfDone(openmittsu::network::ProtocolClient* protocolClient) {
			if (m_messageCount == 0) {
				if (m_hasFailedMessage) {
					LOGGER_DEBUG("Group Message with message ID #{} to group {} has no ACKs outstanding, but we have failed sends.", m_messageId.toString(), m_groupId.toString());
					groupMessageSendFailed(m_groupId, m_messageId, protocolClient);
				} else {
					LOGGER_DEBUG("Group Message with message ID #{} to group {} has no ACKs outstanding, done.", m_messageId.toString(), m_groupId.toString());
					groupMessageSendSuccess(m_groupId, m_messageId, protocolClient);
				}
			} else {
				LOGGER_DEBUG("Group Message with message ID #{} to group {} still has {} ACKs outstanding.", m_messageId.toString(), m_groupId.toString(), m_messageCount);
			}
		}

	}
}
