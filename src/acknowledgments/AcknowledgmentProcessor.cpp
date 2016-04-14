#include "acknowledgments/AcknowledgmentProcessor.h"

#include "ProtocolClient.h"
#include "utility/Logging.h"

AcknowledgmentProcessor::AcknowledgmentProcessor(QDateTime const& timeoutTime) : timeoutTime(timeoutTime) {
	// Intentionally left empty.
}

AcknowledgmentProcessor::~AcknowledgmentProcessor() {
	// Intentionally left empty.
}

QDateTime const& AcknowledgmentProcessor::getTimeoutTime() const {
	return timeoutTime;
}

void AcknowledgmentProcessor::contactMessageSendFailed(ContactId const& receiver, MessageId const& messageId, ProtocolClient* protocolClient) {
	LOGGER()->info("Sending MESSAGE_FAILED for message ID {} send to {}.", messageId.toString(), receiver.toString());
	protocolClient->messageSendFailed(receiver, messageId);
}

void AcknowledgmentProcessor::groupMessageSendFailed(GroupId const& groupId, MessageId const& messageId, ProtocolClient* protocolClient) {
	LOGGER()->info("Sending MESSAGE_FAILED for message ID {} send to group {}.", messageId.toString(), groupId.toString());
	protocolClient->messageSendFailed(groupId, messageId);
}

void AcknowledgmentProcessor::contactMessageSendSuccess(ContactId const& receiver, MessageId const& messageId, ProtocolClient* protocolClient) {
	LOGGER()->info("Sending MESSAGE_SUCCESS for message ID {} send to {}.", messageId.toString(), receiver.toString());
	protocolClient->messageSendDone(receiver, messageId);
}

void AcknowledgmentProcessor::groupMessageSendSuccess(GroupId const& groupId, MessageId const& messageId, ProtocolClient* protocolClient) {
	LOGGER()->info("Sending MESSAGE_SUCESS for message ID {} send to group {}.", messageId.toString(), groupId.toString());
	protocolClient->messageSendDone(groupId, messageId);
}

void AcknowledgmentProcessor::emitGroupCreationFailed(GroupId const& groupId, ProtocolClient* procolClient) {
	procolClient->groupSetupDone(groupId, false);
}

void AcknowledgmentProcessor::emitGroupCreationSuccess(GroupId const& groupId, ProtocolClient* procolClient) {
	procolClient->groupSetupDone(groupId, true);
}
