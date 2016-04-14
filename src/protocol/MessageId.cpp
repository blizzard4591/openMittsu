#include "protocol/MessageId.h"

#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

#include <QHash>

MessageId::MessageId() : messageId(0) {
	 // Used by the QMetaType system
}

MessageId::MessageId(quint64 messageId) : messageId(messageId) {
	// Intentionally left empty.
}

MessageId::MessageId(QByteArray const& messageIdBytes) : messageId(ByteArrayConversions::convert8ByteQByteArrayToQuint64(messageIdBytes)) {
	// Intentionally left empty.
}

MessageId::MessageId(MessageId const& other) : messageId(other.messageId) {
	// Intentionally left empty.
}

MessageId::~MessageId() {
	// Intentionally left empty.
}

quint64 MessageId::getMessageId() const {
	return messageId;
}

QByteArray MessageId::getMessageIdAsByteArray() const {
	return ByteArrayConversions::convertQuint64toQByteArray(messageId);
}

int MessageId::getSizeOfMessageIdInBytes() {
	return (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES);
}

bool MessageId::operator ==(MessageId const& other) const {
	return messageId == other.messageId;
}

std::string MessageId::toString() const {
	return QString(getMessageIdAsByteArray().toHex()).toStdString();
}

QString MessageId::toQString() const {
	return QString(getMessageIdAsByteArray().toHex());
}

uint qHash(MessageId const& key, uint seed) {
	return qHash(key.getMessageId(), seed);
}
