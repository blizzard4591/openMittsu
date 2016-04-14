#include "messages/group/GroupLeaveMessageContent.h"

#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupLeaveMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE, new TypedMessageContentFactory<GroupLeaveMessageContent>());


GroupLeaveMessageContent::GroupLeaveMessageContent() : GroupMessageContent(GroupId(0, 0)), leavingContactId(0) {
	// Only accessible and used by the MessageContentFactory.
}

GroupLeaveMessageContent::GroupLeaveMessageContent(GroupId const& groupId, ContactId const& leavingContactId) : GroupMessageContent(groupId), leavingContactId(leavingContactId) {
	// Intentionally left empty.
}

GroupLeaveMessageContent::~GroupLeaveMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupLeaveMessageContent::clone() const {
	return new GroupLeaveMessageContent(getGroupId(), leavingContactId);
}

ContactId const& GroupLeaveMessageContent::getLeavingContactId() const {
	return leavingContactId;
}

MessageContent* GroupLeaveMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE, 1 + GroupId::getSizeOfGroupIdInBytes(), payload);

	GroupId const groupId(GroupId::fromData(payload.mid(1, GroupId::getSizeOfGroupIdInBytes())));

	return new GroupLeaveMessageContent(groupId, messageHeader.getSender());
}

QByteArray GroupLeaveMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE);
	result.append(getGroupId().getGroupIdAsByteArray());

	return result;
}
