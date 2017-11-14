#include "src/protocol/MessageId.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"

#include <QHash>

#include <sodium.h>

namespace openmittsu {
	namespace protocol {

		MessageId::MessageId() : messageId(0), isValid(false) {
			// Used by the QMetaType system
		}

		MessageId::MessageId(quint64 messageId) : messageId(messageId), isValid(true) {
			// Intentionally left empty.
		}

		MessageId::MessageId(QByteArray const& messageIdBytes) : messageId(openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(messageIdBytes)), isValid(true) {
			// Intentionally left empty.
		}

		MessageId::MessageId(QString const& messageIdString) : messageId(openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(QByteArray::fromHex(messageIdString.toUtf8()))), isValid(true) {
			// Intentionally left empty.
		}

		MessageId::MessageId(MessageId const& other) : messageId(other.messageId), isValid(other.isValid) {
			// Intentionally left empty.
		}

		MessageId::~MessageId() {
			// Intentionally left empty.
		}

		void MessageId::throwIfInvalid() const {
			if (!isValid) {
				throw openmittsu::exceptions::InternalErrorException() << "A default non-random Message ID is about to be used, this should never happen.";
			}
		}

		quint64 MessageId::getMessageId() const {
			throwIfInvalid();

			return messageId;
		}

		QByteArray MessageId::getMessageIdAsByteArray() const {
			throwIfInvalid();

			return openmittsu::utility::ByteArrayConversions::convertQuint64toQByteArray(messageId);
		}

		int MessageId::getSizeOfMessageIdInBytes() {
			return (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES);
		}

		bool MessageId::operator ==(MessageId const& other) const {
			return (messageId == other.messageId) && (isValid == other.isValid);
		}

		bool MessageId::operator !=(MessageId const& other) const {
			return !((*this) == other);
		}

		std::string MessageId::toString() const {
			throwIfInvalid();

			return QString(getMessageIdAsByteArray().toHex()).toStdString();
		}

		QString MessageId::toQString() const {
			throwIfInvalid();

			return QString(getMessageIdAsByteArray().toHex());
		}

		MessageId MessageId::random() {
			QByteArray messageIdBytes(MessageId::getSizeOfMessageIdInBytes(), 0x00);

			do {
				randombytes_buf(messageIdBytes.data(), MessageId::getSizeOfMessageIdInBytes());
			} while (openmittsu::utility::ByteArrayConversions::convert8ByteQByteArrayToQuint64(messageIdBytes) == 0);

			return MessageId(messageIdBytes);
		}


		uint qHash(MessageId const& key, uint seed) {
			return ::qHash(key.getMessageId(), seed);
		}

	}
}
