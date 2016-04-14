#include "acknowledgments/GroupCreationMessageAcknowledgmentProcessor.h"

#include "MessageCenter.h"
#include "ProtocolClient.h"
#include "utility/Logging.h"

GroupCreationMessageAcknowledgmentProcessor::GroupCreationMessageAcknowledgmentProcessor(GroupId const& groupId, QDateTime const& timeoutTime, MessageId const& groupUniqueMessageId, bool informViaSignal) : GroupContentMessageAcknowledgmentProcessor(groupId, timeoutTime, groupUniqueMessageId), informViaSignal(informViaSignal) {
	// Intentionally left empty.
}

GroupCreationMessageAcknowledgmentProcessor::~GroupCreationMessageAcknowledgmentProcessor() {
	// Intentionally left empty.
}

void GroupCreationMessageAcknowledgmentProcessor::sendResultIfDone(ProtocolClient* protocolClient) {
	if (messages.size() == 0) {
		if (informViaSignal) {
			if (hasFailedMessage) {
				emitGroupCreationFailed(getGroupId(), protocolClient);
			} else {
				emitGroupCreationSuccess(getGroupId(), protocolClient);
			}
		} else {
			if (hasFailedMessage) {
				LOGGER()->warn("Failed to send group control message with unique ID {} to group {}, will fail silently.", getGroupUniqueMessageId().toString(), getGroupId().toString());
			} else {
				LOGGER()->info("Sent group control message with unique ID {} to group {}, success.", getGroupUniqueMessageId().toString(), getGroupId().toString());
			}
		}
	} else {
		LOGGER_DEBUG("Group Control Message with unique ID {} to group {} still has {} ACKs outstanding.", getGroupUniqueMessageId().toString(), getGroupId().toString(), messages.size());
	}
}
