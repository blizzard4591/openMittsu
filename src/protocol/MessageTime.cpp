#include "protocol/MessageTime.h"

#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"
#include "Endian.h"

#include <QHash>

MessageTime::MessageTime() : messageTime(QDateTime::currentDateTime()) {
	// Used by the QMetaType system
}

MessageTime::MessageTime(QDateTime const& messageTime) : messageTime(messageTime) {
	// Intentionally left empty.
}

MessageTime::MessageTime(QByteArray const& messageTimeBytes) : messageTime(QDateTime::fromTime_t(Endian::uint32FromLittleEndianToHostEndian(ByteArrayConversions::convert4ByteQByteArrayToQuint32(messageTimeBytes)))) {
	// Intentionally left empty.
}

MessageTime::MessageTime(MessageTime const& other) : messageTime(other.messageTime) {
	// Intentionally left empty.
}

MessageTime::~MessageTime() {
	// Intentionally left empty.
}

MessageTime MessageTime::now() {
	return MessageTime();
}

quint64 MessageTime::getMessageTime() const {
	return messageTime.toTime_t();
}

QDateTime const& MessageTime::getTime() const {
	return messageTime;
}

QByteArray MessageTime::getMessageTimeAsByteArray() const {
	return ByteArrayConversions::convertQuint32toQByteArray(Endian::uint32FromHostEndianToLittleEndian(static_cast<quint32>(messageTime.toTime_t())));
}

int MessageTime::getSizeOfMessageTimeInBytes() {
	return (PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES);
}

bool MessageTime::operator ==(MessageTime const& other) const {
	return messageTime == other.messageTime;
}

std::string MessageTime::toString() const {
	return messageTime.toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy")).toStdString();
}

QString MessageTime::toQString() const {
	return messageTime.toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"));
}

uint qHash(MessageTime const& key, uint seed) {
	return qHash(key.getTime(), seed);
}
