#include "messages/group/GroupSyncMessageContent.h"

#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupSyncMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_SYNC, new TypedMessageContentFactory<GroupSyncMessageContent>());


GroupSyncMessageContent::GroupSyncMessageContent() : GroupMessageContent(GroupId(0, 0)) {
	// Only accessible and used by the MessageContentFactory.
}

GroupSyncMessageContent::GroupSyncMessageContent(GroupId const& groupId) : GroupMessageContent(groupId) {
	// Intentionally left empty.
}

GroupSyncMessageContent::~GroupSyncMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupSyncMessageContent::clone() const {
	return new GroupSyncMessageContent(getGroupId());
}

MessageContent* GroupSyncMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	int const pureGroupIdSize = GroupId::getSizeOfGroupIdInBytes() - ContactId::getSizeOfContactIdInBytes();
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_SYNC, 1 + pureGroupIdSize, payload, true);

	quint64 const pureGroupId = ByteArrayConversions::convert8ByteQByteArrayToQuint64(payload.mid(1, pureGroupIdSize));

	GroupId const groupId(messageHeader.getReceiver(), pureGroupId);

	return new GroupSyncMessageContent(groupId);
}

QByteArray GroupSyncMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_SYNC);

	quint64 const pureGroupId = getGroupId().getGroupId();
	QByteArray const pureGroupIdBytes = ByteArrayConversions::convertQuint64toQByteArray(pureGroupId);

	result.append(pureGroupIdBytes);

	return result;
}
