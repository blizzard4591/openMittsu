#include "protocol/ContactId.h"

#include "Endian.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/ByteArrayConversions.h"

#include <QHash>

ContactId::ContactId() : contactId(0) {
	// Used by the QMetaType system
}

ContactId::ContactId(quint64 contactId) : contactId(contactId) {
	// Intentionally left empty.
}

ContactId::ContactId(QString const& identityString) : contactId(ByteArrayConversions::convert8ByteQByteArrayToQuint64(identityString.toUtf8())) {
	// Intentionally left empty.
}

ContactId::ContactId(QByteArray const& contactIdBytes) : contactId(ByteArrayConversions::convert8ByteQByteArrayToQuint64(contactIdBytes)) {
	// Intentionally left empty.
}

ContactId::ContactId(ContactId const& other) : contactId(other.contactId) {
	// Intentionally left empty.
}

ContactId::~ContactId() {
	// Intentionally left empty.
}

quint64 ContactId::getContactId() const {
	return contactId;
}

QByteArray ContactId::getContactIdAsByteArray() const {
	return ByteArrayConversions::convertQuint64toQByteArray(contactId);
}

int ContactId::getSizeOfContactIdInBytes() {
	return (PROTO_IDENTITY_LENGTH_BYTES);
}

bool ContactId::operator ==(ContactId const& other) const {
	return contactId == other.contactId;
}

bool ContactId::operator <(ContactId const& other) const {
	// This operator is used to define the sorting order of ContactIds.
	// The ordering is a lexicographical ordering on the String representations of the IDs.
	// Because the first character is the most significant character in this ordering, we interpret the byte sequence as being a big endian number.
	return Endian::uint64FromBigEndianToHostEndian(contactId) < Endian::uint64FromBigEndianToHostEndian(other.contactId);
}

std::string ContactId::toString() const {
	return QString(getContactIdAsByteArray()).toStdString();
}

QString ContactId::toQString() const {
	return QString(getContactIdAsByteArray());
}

uint qHash(ContactId const& key, uint seed) {
	return qHash(key.getContactId(), seed);
}
