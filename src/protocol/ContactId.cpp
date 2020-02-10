#include "src/protocol/ContactId.h"

#include "src/utility/Endian.h"
#include "src/utility/ByteArrayConversions.h"

#include <QHash>

namespace openmittsu {
	namespace protocol {

		ContactId::ContactId() : contactId(0) {
			// Used by the QMetaType system
		}

		ContactId::ContactId(quint64 contactId) : contactId(contactId) {
			// Intentionally left empty.
		}

		ContactId::ContactId(QString const& identityString) : contactId(openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(identityString.toUtf8())) {
			// Intentionally left empty.
		}

		ContactId::ContactId(QByteArray const& contactIdBytes) : contactId(openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(contactIdBytes)) {
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
			return openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(contactId);
		}

		bool ContactId::operator ==(ContactId const& other) const {
			return contactId == other.contactId;
		}

		bool ContactId::operator !=(ContactId const& other) const {
			return contactId != other.contactId;
		}

		bool ContactId::operator <(ContactId const& other) const {
			// This operator is used to define the sorting order of ContactIds.
			// The ordering is a lexicographical ordering on the String representations of the IDs.
			// Because the first character is the most significant character in this ordering, we interpret the byte sequence as being a big endian number.
			return openmittsu::utility::Endian::uint64FromBigEndianToHostEndian(contactId) < openmittsu::utility::Endian::uint64FromBigEndianToHostEndian(other.contactId);
		}

		std::string ContactId::toString() const {
			return QString(getContactIdAsByteArray()).toStdString();
		}

		QString ContactId::toQString() const {
			return QString(getContactIdAsByteArray());
		}

		bool ContactId::isValidContactId(QString const& identityString) {
			if (identityString.size() != 8) {
				return false;
			}

			// ^[a-zA-Z0-9]{8}$
			for (int i = 0; i < identityString.size(); ++i) {
				char latinChar = identityString.at(i).toLatin1();
				if (!(((latinChar >= 'a') && (latinChar <= 'z')) || ((latinChar >= 'A') && (latinChar <= 'Z')) || ((latinChar >= '0') && (latinChar <= '9')))) {
					return false;
				}
			}

			return true;
		}

		uint qHash(ContactId const& key, uint seed) {
			return ::qHash(key.getContactId(), seed);
		}

	}
}
