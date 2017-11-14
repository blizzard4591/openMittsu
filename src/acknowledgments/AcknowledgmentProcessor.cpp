#include "src/acknowledgments/AcknowledgmentProcessor.h"

#include "src/network/ProtocolClient.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace acknowledgments {

		AcknowledgmentProcessor::AcknowledgmentProcessor(QDateTime const& timeoutTime) : m_timeoutTime(timeoutTime) {
			// Intentionally left empty.
		}

		AcknowledgmentProcessor::~AcknowledgmentProcessor() {
			// Intentionally left empty.
		}

		QDateTime const& AcknowledgmentProcessor::getTimeoutTime() const {
			return m_timeoutTime;
		}

		void AcknowledgmentProcessor::contactMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient) {
			LOGGER()->info("Sending MESSAGE_FAILED for message ID {} send to {}.", messageId.toString(), receiver.toString());
			protocolClient->messageSendFailed(receiver, messageId);
		}

		void AcknowledgmentProcessor::groupMessageSendFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient) {
			LOGGER()->info("Sending MESSAGE_FAILED for message ID {} send to group {}.", messageId.toString(), groupId.toString());
			protocolClient->messageSendFailed(groupId, messageId);
		}

		void AcknowledgmentProcessor::contactMessageSendSuccess(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient) {
			LOGGER()->info("Sending MESSAGE_SUCCESS for message ID {} send to {}.", messageId.toString(), receiver.toString());
			protocolClient->messageSendDone(receiver, messageId);
		}

		void AcknowledgmentProcessor::groupMessageSendSuccess(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient) {
			LOGGER()->info("Sending MESSAGE_SUCESS for message ID {} send to group {}.", messageId.toString(), groupId.toString());
			protocolClient->messageSendDone(groupId, messageId);
		}

		void AcknowledgmentProcessor::emitGroupCreationFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::network::ProtocolClient* procolClient) {
			procolClient->groupSetupDone(groupId, false);
		}

		void AcknowledgmentProcessor::emitGroupCreationSuccess(openmittsu::protocol::GroupId const& groupId, openmittsu::network::ProtocolClient* procolClient) {
			procolClient->groupSetupDone(groupId, true);
		}

	}
}
