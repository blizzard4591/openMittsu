#include "src/acknowledgments/GroupCreationMessageAcknowledgmentProcessor.h"

#include "src/network/ProtocolClient.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace acknowledgments {

		GroupCreationMessageAcknowledgmentProcessor::GroupCreationMessageAcknowledgmentProcessor(openmittsu::protocol::GroupId const& groupId, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& groupUniqueMessageId, bool informViaSignal) : GroupContentMessageAcknowledgmentProcessor(groupId, timeoutTime, groupUniqueMessageId), m_informViaSignal(informViaSignal) {
			// Intentionally left empty.
		}

		GroupCreationMessageAcknowledgmentProcessor::~GroupCreationMessageAcknowledgmentProcessor() {
			// Intentionally left empty.
		}

		void GroupCreationMessageAcknowledgmentProcessor::sendResultIfDone(openmittsu::network::ProtocolClient* protocolClient) {
			if (m_messageCount == 0) {
				if (m_informViaSignal) {
					if (m_hasFailedMessage) {
						emitGroupCreationFailed(getGroupId(), protocolClient);
					} else {
						emitGroupCreationSuccess(getGroupId(), protocolClient);
					}
				} else {
					if (m_hasFailedMessage) {
						LOGGER()->warn("Failed to send group control message with message ID #{} to group {}, will fail silently.", getGroupUniqueMessageId().toString(), getGroupId().toString());
					} else {
						LOGGER()->info("Sent group control message with message ID #{} to group {}, success.", getGroupUniqueMessageId().toString(), getGroupId().toString());
					}
				}
			} else {
				LOGGER_DEBUG("Group Control Message with message ID #{} to group {} still has {} ACKs outstanding.", getGroupUniqueMessageId().toString(), getGroupId().toString(), m_messageCount);
			}
		}

	}
}
