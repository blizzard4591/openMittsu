#include "protocol/PushFromId.h"

#include "exceptions/IllegalArgumentException.h"
#include "protocol/ContactId.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

PushFromId::PushFromId() : pushFromIdBytes(PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES, 0x00) {
	throw;
}

PushFromId::PushFromId(ContactId const& contactId) : pushFromIdBytes(expandFromContactId(contactId)) {
	// Intentionally left empty.
}

PushFromId::PushFromId(QString const& selfNickname) : pushFromIdBytes(expandFromNickname(selfNickname)) {
	// Intentionally left empty.
}

PushFromId::PushFromId(QByteArray const& pushFromId) : pushFromIdBytes(pushFromId) {
	if (pushFromIdBytes.size() != (PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES)) {
		throw IllegalArgumentException() << "Size of PushFromId is " << pushFromIdBytes.size() << " instead of " << (PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES) << " Bytes.";
	}
}

PushFromId::PushFromId(PushFromId const& other) : pushFromIdBytes(other.pushFromIdBytes) {
	// Intentionally left empty.
}

PushFromId::~PushFromId() {
	// Intentionally left empty.
}

bool PushFromId::containsStandardContactId() const {
	QByteArray const nonIdPart(pushFromIdBytes.right((PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES) - (PROTO_IDENTITY_LENGTH_BYTES)));

	int const size = nonIdPart.size();
	for (int i = 0; i < size; ++i) {
		if (nonIdPart.at(i) != 0x00) {
			return false;
		}
	}

	return true;
}

quint64 PushFromId::getPushFromId() const {
	return ByteArrayConversions::convert8ByteQByteArrayToQuint64(pushFromIdBytes.left(PROTO_IDENTITY_LENGTH_BYTES));
}

QByteArray PushFromId::getPushFromIdAsByteArray() const {
	return pushFromIdBytes;
}

QByteArray PushFromId::expandFromContactId(ContactId const& contactId) {
	QByteArray result = contactId.getContactIdAsByteArray();
	result.append(QByteArray((PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES - result.size()), 0x00));

	return result;
}

QByteArray PushFromId::expandFromNickname(QString const& nickname) {
	QByteArray result = nickname.toUtf8().left(PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES);
	if (result.size() < (PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES)) {
		result.append(QByteArray((PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES - result.size()), 0x00));
	}

	return result;
}

int PushFromId::getSizeOfPushFromIdInBytes() {
	return (PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES);
}

bool PushFromId::operator ==(PushFromId const& other) const {
	return pushFromIdBytes == other.pushFromIdBytes;
}

std::string PushFromId::toString() const {
	return QString(getPushFromIdAsByteArray().toHex()).toStdString();
}

QString PushFromId::toQString() const {
	return QString(getPushFromIdAsByteArray().toHex());
}
