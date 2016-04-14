#include "acknowledgments/ContactMessageAcknowledgmentProcessor.h"

#include "MessageCenter.h"
#include "ProtocolClient.h"
#include "exceptions/IllegalArgumentException.h"

ContactMessageAcknowledgmentProcessor::ContactMessageAcknowledgmentProcessor(ContactId const& receiver, QDateTime const& timeoutTime, MessageId const& messageId) : AcknowledgmentProcessor(timeoutTime), receiver(receiver), messageId(messageId) {
	// Intentionally left empty.
}

ContactMessageAcknowledgmentProcessor::~ContactMessageAcknowledgmentProcessor() {
	// Intentionally left empty.
}

ContactId const& ContactMessageAcknowledgmentProcessor::getReceiver() const {
	return receiver;
}

MessageId const& ContactMessageAcknowledgmentProcessor::getMessageId() const {
	return messageId;
}

void ContactMessageAcknowledgmentProcessor::sendFailed(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) {
	contactMessageSendFailed(receiver, messageId, protocolClient);
}

void ContactMessageAcknowledgmentProcessor::sendSuccess(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) {
	contactMessageSendSuccess(receiver, messageId, protocolClient);
}

void ContactMessageAcknowledgmentProcessor::addMessage(MessageId const& addedMessageId) {
	if (!(messageId == addedMessageId)) {
		throw IllegalArgumentException() << "Tried adding Message ID " << addedMessageId.toString() << " to ContactMessageAcknowledgmentProcessor built for Message ID " << messageId.toString() << ".";
	}
}
