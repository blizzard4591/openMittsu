#include "messages/group/GroupTextMessageContent.h"

#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupTextMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_TEXT, new TypedMessageContentFactory<GroupTextMessageContent>());


GroupTextMessageContent::GroupTextMessageContent() : GroupMessageContent(GroupId(0, 0)), text() {
	// Only accessible and used by the MessageContentFactory.
}

GroupTextMessageContent::GroupTextMessageContent(GroupId const& groupId, QString const& text) : GroupMessageContent(groupId), text(text) {
	// Intentionally left empty.
}

GroupTextMessageContent::~GroupTextMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupTextMessageContent::clone() const {
	return new GroupTextMessageContent(getGroupId(), text);
}

QString const& GroupTextMessageContent::getText() const {
	return text;
}

MessageContent* GroupTextMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_TEXT, 1 + GroupId::getSizeOfGroupIdInBytes() + 1, payload);

	GroupId const groupId(GroupId::fromData(payload.mid(1, GroupId::getSizeOfGroupIdInBytes())));

	QString payloadText(QString::fromUtf8(payload.mid(1 + GroupId::getSizeOfGroupIdInBytes())));

	return new GroupTextMessageContent(groupId, payloadText);
}

QByteArray GroupTextMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_TEXT);
	result.append(getGroupId().getGroupIdAsByteArray());
	result.append(text.toUtf8());

	return result;
}
