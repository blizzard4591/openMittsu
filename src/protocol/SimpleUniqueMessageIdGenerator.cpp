#include "protocol/SimpleUniqueMessageIdGenerator.h"

#include <sodium.h>

SimpleUniqueMessageIdGenerator::~SimpleUniqueMessageIdGenerator() {
	// Intentionally left empty.
}

MessageId SimpleUniqueMessageIdGenerator::getNextUniqueMessageId(ContactId const& receiver) {
	QByteArray messageIdBytes(MessageId::getSizeOfMessageIdInBytes(), 0x00);

	randombytes_buf(messageIdBytes.data(), MessageId::getSizeOfMessageIdInBytes());

	return MessageId(messageIdBytes);
}

MessageId SimpleUniqueMessageIdGenerator::getNextUniqueMessageId(GroupId const& receiver) {
	QByteArray messageIdBytes(MessageId::getSizeOfMessageIdInBytes(), 0x00);

	randombytes_buf(messageIdBytes.data(), MessageId::getSizeOfMessageIdInBytes());

	return MessageId(messageIdBytes);
}
