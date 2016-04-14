#include "messages/group/GroupCreationMessageContent.h"

#include "exceptions/ProtocolErrorException.h"
#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupCreationMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_CREATION, new TypedMessageContentFactory<GroupCreationMessageContent>());


GroupCreationMessageContent::GroupCreationMessageContent() : GroupMessageContent(GroupId(0, 0)) {
	// Only accessible and used by the MessageContentFactory.
}

GroupCreationMessageContent::GroupCreationMessageContent(GroupId const& groupId, QSet<ContactId> const groupMembers) : GroupMessageContent(groupId), members(groupMembers) {
	// Intentionally left empty.
}

GroupCreationMessageContent::~GroupCreationMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupCreationMessageContent::clone() const {
	return new GroupCreationMessageContent(getGroupId(), members);
}

QSet<ContactId> const& GroupCreationMessageContent::getGroupMembers() const {
	return members;
}

MessageContent* GroupCreationMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	int const pureGroupIdSize = GroupId::getSizeOfGroupIdInBytes() - ContactId::getSizeOfContactIdInBytes();
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_CREATION, 1 + pureGroupIdSize, payload);

	quint64 const pureGroupId = ByteArrayConversions::convert8ByteQByteArrayToQuint64(payload.mid(1, pureGroupIdSize));

	GroupId const groupId(messageHeader.getSender(), pureGroupId);

	QByteArray const memberIds = payload.mid(1 + pureGroupIdSize);
	if ((memberIds.size() % ContactId::getSizeOfContactIdInBytes()) != 0) {
		throw ProtocolErrorException() << "Can not decode Group Creation Message where the member id list size is not a multiple of the size of a single ID. Payload: " << QString(payload.toHex()).toStdString();
	}
	
	int const memberCount = memberIds.size() / ContactId::getSizeOfContactIdInBytes();
	QSet<ContactId> groupMembers;
	for (int i = 0; i < memberCount; ++i) {
		ContactId const memberId(memberIds.mid(i * ContactId::getSizeOfContactIdInBytes(), ContactId::getSizeOfContactIdInBytes()));
		groupMembers.insert(memberId);
	}

	return new GroupCreationMessageContent(groupId, groupMembers);
}

QByteArray GroupCreationMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_CREATION);

	quint64 const pureGroupId = getGroupId().getGroupId();
	QByteArray const pureGroupIdBytes = ByteArrayConversions::convertQuint64toQByteArray(pureGroupId);

	result.append(pureGroupIdBytes);

	QSet<ContactId>::const_iterator it = members.constBegin();
	QSet<ContactId>::const_iterator end = members.constEnd();
	for (; it != end; ++it) {
		result.append(it->getContactIdAsByteArray());
	}

	return result;
}
