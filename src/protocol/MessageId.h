#ifndef OPENMITTSU_PROTOCOL_MESSAGEID_H_
#define OPENMITTSU_PROTOCOL_MESSAGEID_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QVector>
#include <QString>

namespace openmittsu {
	namespace database {
		class DatabaseWrapper;
	}

	namespace protocol {

		class MessageId {
		public:
			explicit MessageId(quint64 messageId);
			explicit MessageId(QByteArray const& messageIdBytes);
			explicit MessageId(QString const& messageIdString);

			MessageId(MessageId const& other);
			virtual ~MessageId();

			quint64 getMessageId() const;
			QByteArray getMessageIdAsByteArray() const;
			static int getSizeOfMessageIdInBytes();
			std::string toString() const;
			QString toQString() const;

			bool operator ==(MessageId const& other) const;
			bool operator !=(MessageId const& other) const;

			static MessageId random();

			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<MessageId, true>;
			friend class QVector<MessageId>;
			friend class openmittsu::database::DatabaseWrapper;
		private:
			quint64 messageId;
			bool isValid;
			void throwIfInvalid() const;

			MessageId();
		};

		uint qHash(MessageId const& key, uint seed);
	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::MessageId)

#endif // #define OPENMITTSU_PROTOCOL_MESSAGEID_H_

