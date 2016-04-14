#include "acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"

#include "MessageCenter.h"
#include "ProtocolClient.h"
#include "exceptions/IllegalArgumentException.h"
#include "utility/Logging.h"

GroupContentMessageAcknowledgmentProcessor::GroupContentMessageAcknowledgmentProcessor(GroupId const& groupId, QDateTime const& timeoutTime, MessageId const& groupUniqueMessageId) : AcknowledgmentProcessor(timeoutTime), messages(), hasFailedMessage(false), groupId(groupId), groupUniqueMessageId(groupUniqueMessageId) {
	// Intentionally left empty.
}

GroupContentMessageAcknowledgmentProcessor::~GroupContentMessageAcknowledgmentProcessor() {
	// Intentionally left empty.
}

GroupId const& GroupContentMessageAcknowledgmentProcessor::getGroupId() const {
	return groupId;
}

MessageId const& GroupContentMessageAcknowledgmentProcessor::getGroupUniqueMessageId() const {
	return groupUniqueMessageId;
}

void GroupContentMessageAcknowledgmentProcessor::addMessage(MessageId const& addedMessageId) {
	if (messages.contains(addedMessageId)) {
		throw IllegalArgumentException() << "Tried adding Message ID " << addedMessageId.toString() << " to GroupContentMessageAcknowledgmentProcessor that already contains this Message ID.";
	}
	messages.insert(addedMessageId);
}

void GroupContentMessageAcknowledgmentProcessor::sendFailed(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) {
	if (messages.contains(messageId)) {
		hasFailedMessage = true;
		messages.remove(messageId);
		sendResultIfDone(protocolClient);
	} else {
		LOGGER()->warn("GroupContentMessageAcknowledgmentProcessor received a MESSAGE_FAILED notification for group {}, unique message ID {}, send with message ID {}, that was not recognized.", groupId.toString(), groupUniqueMessageId.toString(), messageId.toString());
	}
}

void GroupContentMessageAcknowledgmentProcessor::sendSuccess(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) {
	if (messages.contains(messageId)) {
		messages.remove(messageId);
		sendResultIfDone(protocolClient);
	} else {
		LOGGER()->warn("GroupContentMessageAcknowledgmentProcessor received a MESSAGE_SUCCESS notification for group {}, unique message ID {}, send with message ID {}, that was not recognized.", groupId.toString(), groupUniqueMessageId.toString(), messageId.toString());
	}
}

void GroupContentMessageAcknowledgmentProcessor::sendResultIfDone(ProtocolClient* protocolClient) {
	if (messages.size() == 0) {
		if (hasFailedMessage) {
			groupMessageSendFailed(groupId, groupUniqueMessageId, protocolClient);
		} else {
			groupMessageSendSuccess(groupId, groupUniqueMessageId, protocolClient);
		}
	} else {
		LOGGER_DEBUG("Group Message with unique ID {} to group {} still has {} ACKs outstanding.", groupUniqueMessageId.toString(), groupId.toString(), messages.size());
	}
}
