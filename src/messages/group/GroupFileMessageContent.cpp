#include "messages/group/GroupFileMessageContent.h"

#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/Logging.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupFileMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_FILE, new TypedMessageContentFactory<GroupFileMessageContent>());


GroupFileMessageContent::GroupFileMessageContent() : GroupMessageContent(GroupId(0, 0)) {
	// Only accessible and used by the MessageContentFactory.
}

GroupFileMessageContent::GroupFileMessageContent(GroupId const& groupId) : GroupMessageContent(groupId) {
	// Intentionally left empty.
}

GroupFileMessageContent::~GroupFileMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupFileMessageContent::clone() const {
	return new GroupFileMessageContent(getGroupId());
}

MessageContent* GroupFileMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_FILE, 1 + GroupId::getSizeOfGroupIdInBytes() + 1, payload);

	GroupId const groupId(GroupId::fromData(payload.mid(1, GroupId::getSizeOfGroupIdInBytes())));

	// TODO
	QByteArray const data = (payload.mid(1 + GroupId::getSizeOfGroupIdInBytes()));
	LOGGER_DEBUG("Received Group File with payload {}", QString(data.toHex()).toStdString());

	return new GroupFileMessageContent(groupId);
}

QByteArray GroupFileMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_FILE);
	result.append(getGroupId().getGroupIdAsByteArray());

	// TODO

	return result;
}
