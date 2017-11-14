#include "src/messages/group/GroupLeaveMessageContent.h"

#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupLeaveMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE, new TypedMessageContentFactory<GroupLeaveMessageContent>());


			GroupLeaveMessageContent::GroupLeaveMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)), leavingContactId(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupLeaveMessageContent::GroupLeaveMessageContent(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::ContactId const& leavingContactId) : GroupMessageContent(groupId), leavingContactId(leavingContactId) {
				// Intentionally left empty.
			}

			GroupLeaveMessageContent::~GroupLeaveMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupLeaveMessageContent::clone() const {
				return new GroupLeaveMessageContent(getGroupId(), leavingContactId);
			}

			openmittsu::protocol::ContactId const& GroupLeaveMessageContent::getLeavingContactId() const {
				return leavingContactId;
			}

			MessageContent* GroupLeaveMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE, 1 + openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes(), payload);

				openmittsu::protocol::GroupId const groupId(openmittsu::protocol::GroupId::fromData(payload.mid(1, openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes())));

				return new GroupLeaveMessageContent(groupId, messageHeader.getSender());
			}

			QByteArray GroupLeaveMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE);
				result.append(getGroupId().getGroupIdAsByteArray());

				return result;
			}

		}
	}
}
