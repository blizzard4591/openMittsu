#include "messages/FullMessageHeader.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/IllegalFunctionCallException.h"
#include "exceptions/InternalErrorException.h"

#include "messages/PreliminaryMessageHeader.h"

#include "messages/contact/PreliminaryContactMessageHeader.h"
#include "messages/group/PreliminaryGroupMessageHeader.h"

#include "protocol/Nonce.h"
#include "protocol/ProtocolSpecs.h"

#include "utility/Logging.h"

FullMessageHeader::FullMessageHeader() : sender(0), receiver(0), messageId(0), time(MessageTime::now()), flags(), pushFromName(ContactId(0)) {
	throw;
}

FullMessageHeader::FullMessageHeader(ContactId const& receiverId, MessageTime const& time, ContactId const& sender, MessageId const& messageId, MessageFlags const& flags, PushFromId const& pushFromId) : sender(sender), receiver(receiverId), messageId(messageId), time(time), flags(flags), pushFromName(pushFromId) {
	// Intentionally left empty.
}

FullMessageHeader::FullMessageHeader(PreliminaryContactMessageHeader const* preliminaryHeader, ContactId const& sender, PushFromId const& pushFromId) : sender(sender), receiver(preliminaryHeader->getReceiver()), messageId(preliminaryHeader->getMessageId()), time(preliminaryHeader->getTime()), flags(preliminaryHeader->getFlags()), pushFromName(pushFromId) {
	// Intentionally left empty.
}

FullMessageHeader::FullMessageHeader(PreliminaryGroupMessageHeader const* preliminaryHeader, ContactId const& sender, PushFromId const& pushFromId) : sender(sender), receiver(ContactId(0)), messageId(preliminaryHeader->getMessageId()), time(preliminaryHeader->getTime()), flags(preliminaryHeader->getFlags()), pushFromName(pushFromId) {
	// Intentionally left empty.
}

FullMessageHeader::FullMessageHeader(FullMessageHeader const& messageHeader, ContactId const& replacementReceiver, MessageId const& replacementMessageId) : sender(messageHeader.sender), receiver(replacementReceiver), messageId(replacementMessageId), time(messageHeader.time), flags(messageHeader.flags), pushFromName(messageHeader.pushFromName) {
	// Intentionally left empty.
}

FullMessageHeader::FullMessageHeader(FullMessageHeader const& other) : sender(other.sender), receiver(other.receiver), messageId(other.messageId), time(other.time), flags(other.flags), pushFromName(other.pushFromName) {
	// Intentionally left empty.
}

FullMessageHeader::~FullMessageHeader() {
	// Intentionally left empty.
}

int FullMessageHeader::getFullMessageHeaderSize() {
	return (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES);
}

FullMessageHeader FullMessageHeader::fromHeaderData(QByteArray const& headerData) {
	if (headerData.size() != (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES)) {
		throw IllegalArgumentException() << "Size of Header data segment is " << headerData.size() << " instead of " << (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES) << " Bytes.";
	}

	int startPosition = 0;
	ContactId const senderId(headerData.mid(startPosition, PROTO_IDENTITY_LENGTH_BYTES));
	startPosition += PROTO_IDENTITY_LENGTH_BYTES;
	ContactId const receiverId(headerData.mid(startPosition, PROTO_IDENTITY_LENGTH_BYTES));
	startPosition += PROTO_IDENTITY_LENGTH_BYTES;
	MessageId const messageId(headerData.mid(startPosition, PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES));
	startPosition += PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES;
	MessageTime const timestamp(headerData.mid(startPosition, PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES));
	LOGGER_DEBUG("Incoming Message has raw MessageTime bytes {}, MessageTime has bytes {} with time_t {} which is {}.", QString(headerData.mid(startPosition, PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES).toHex()).toStdString(), QString(timestamp.getMessageTimeAsByteArray().toHex()).toStdString(), timestamp.getMessageTime(), timestamp.toString());
	LOGGER_DEBUG("Local MessageTime has bytes {} with time_t {} which is {}.", QString(MessageTime::now().getMessageTimeAsByteArray().toHex()).toStdString(), MessageTime::now().getMessageTime(), MessageTime::now().toString());
	startPosition += PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES;
	MessageFlags const flags(headerData.at(startPosition));
	startPosition += (PROTO_MESSAGE_FLAGS_LENGTH_BYTES + PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES);
	PushFromId const pushFromId(headerData.mid(startPosition, PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES));
	startPosition += PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES;

	return FullMessageHeader(receiverId, timestamp, senderId, messageId, flags, pushFromId);
}

ContactId const& FullMessageHeader::getSender() const {
	return sender;
}

ContactId const& FullMessageHeader::getReceiver() const {
	if (receiver.getContactId() == 0) {
		throw IllegalFunctionCallException() << "Called getReceiver() on a FullMessageHeader which is used for an UnspecializedGroupMessage.";
	}

	return receiver;
}

MessageId const& FullMessageHeader::getMessageId() const {
	return messageId;
}

MessageTime const& FullMessageHeader::getTime() const {
	return time;
}

MessageFlags const& FullMessageHeader::getFlags() const {
	return flags;
}

PushFromId const& FullMessageHeader::getPushFromName() const {
	return pushFromName;
}

QByteArray FullMessageHeader::toPacket() const {
	QByteArray result;

	result.append(sender.getContactIdAsByteArray());
	result.append(receiver.getContactIdAsByteArray());
	result.append(messageId.getMessageIdAsByteArray());
	result.append(time.getMessageTimeAsByteArray());
	result.append(flags.getFlags());
	result.append(QByteArray(PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES, 0x00));
	result.append(pushFromName.getPushFromIdAsByteArray());

	if (result.size() != (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES)) {
		throw InternalErrorException() << "Size of Header data segment is " << result.size() << " instead of " << (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES) << " Bytes.";
	}

	return result;
}
