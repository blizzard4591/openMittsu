#include "src/protocol/MessageTime.h"

#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Endian.h"
#include "src/utility/QtVersions.h"

#include <QHash>

namespace openmittsu {
	namespace protocol {

		MessageTime::MessageTime() : messageTime(QDateTime::currentDateTime()), null(true) {
			// Used by the QMetaType system
		}

		MessageTime::MessageTime(QDateTime const& messageTime) : messageTime(messageTime), null(false) {
			// Intentionally left empty.
		}

		MessageTime::MessageTime(QByteArray const& messageTimeBytes) : messageTime(
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
			QDateTime::fromSecsSinceEpoch(
#else
			QDateTime::fromMSecsSinceEpoch(1000 *
#endif
			
			openmittsu::utility::Endian::uint32FromLittleEndianToHostEndian(openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(messageTimeBytes)))), null(false) {
			// Intentionally left empty.
		}

		MessageTime::MessageTime(MessageTime const& other) : messageTime(other.messageTime), null(other.null) {
			// Intentionally left empty.
		}

		MessageTime::~MessageTime() {
			// Intentionally left empty.
		}

		MessageTime MessageTime::now() {
			return MessageTime(QDateTime::currentDateTime());
		}

		MessageTime MessageTime::fromDatabase(qint64 msecs) {
			if (msecs > 0) {
				return MessageTime(QDateTime::fromMSecsSinceEpoch(msecs));
			} else {
				return MessageTime();
			}
		}

		quint64 MessageTime::getMessageTime() const {
			return static_cast<quint64>(messageTime.toMSecsSinceEpoch() / 1000);
		}

		qint64 MessageTime::getMessageTimeMSecs() const {
			return messageTime.toMSecsSinceEpoch();
		}

		QDateTime const& MessageTime::getTime() const {
			return messageTime;
		}

		QByteArray MessageTime::getMessageTimeAsByteArray() const {
			return openmittsu::utility::ByteArrayConversions::convertQuint32toQByteArray(openmittsu::utility::Endian::uint32FromHostEndianToLittleEndian(static_cast<quint32>(
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
				messageTime.toSecsSinceEpoch()
#else
				(messageTime.toMSecsSinceEpoch() / 1000)
#endif
				)));
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

		bool MessageTime::isNull() const {
			return null;
		}

		utility::QtHashSizeType qHash(MessageTime const& key, utility::QtHashSizeType seed) {
			return qHash(key.getTime(), seed);
		}
	}
}

