#include "messages/group/GroupSetTitleMessageContent.h"

#include "messages/MessageContentRegistry.h"
#include "protocol/GroupId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupSetTitleMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_TITLE, new TypedMessageContentFactory<GroupSetTitleMessageContent>());


GroupSetTitleMessageContent::GroupSetTitleMessageContent() : GroupMessageContent(GroupId(0, 0)), title() {
	// Only accessible and used by the MessageContentFactory.
}

GroupSetTitleMessageContent::GroupSetTitleMessageContent(GroupId const& groupId, QString const& title) : GroupMessageContent(groupId), title(title) {
	// Intentionally left empty.
}

GroupSetTitleMessageContent::~GroupSetTitleMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupSetTitleMessageContent::clone() const {
	return new GroupSetTitleMessageContent(getGroupId(), title);
}

QString const& GroupSetTitleMessageContent::getTitle() const {
	return title;
}

MessageContent* GroupSetTitleMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	int const pureGroupIdSize = GroupId::getSizeOfGroupIdInBytes() - ContactId::getSizeOfContactIdInBytes();
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_TITLE, 1 + pureGroupIdSize, payload);

	quint64 const pureGroupId = ByteArrayConversions::convert8ByteQByteArrayToQuint64(payload.mid(1, pureGroupIdSize));

	GroupId const groupId(messageHeader.getSender(), pureGroupId);

	QString payloadText(QString::fromUtf8(payload.mid(1 + pureGroupIdSize)));

	return new GroupSetTitleMessageContent(groupId, payloadText);
}

QByteArray GroupSetTitleMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_TITLE);
	
	quint64 const pureGroupId = getGroupId().getGroupId();
	QByteArray const pureGroupIdBytes = ByteArrayConversions::convertQuint64toQByteArray(pureGroupId);

	result.append(pureGroupIdBytes);

	result.append(title.toUtf8());

	return result;
}
