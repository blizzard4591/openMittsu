#ifndef OPENMITTSU_PROTOCOL_MESSAGETIME_H_
#define OPENMITTSU_PROTOCOL_MESSAGETIME_H_

#include "src/utility/QtVersions.h"

#include <cstdint>
#include <QtGlobal>
#include <QDateTime>
#include <QByteArray>
#include <QMetaType>

namespace openmittsu {
	namespace protocol {

		class MessageTime {
		public:
			explicit MessageTime();
			explicit MessageTime(QDateTime const& messageTime);
			explicit MessageTime(QByteArray const& messageTimeBytes);
			MessageTime(MessageTime const& other);
			virtual ~MessageTime();

			quint64 getMessageTime() const;
			qint64 getMessageTimeMSecs() const;
			QDateTime const& getTime() const;
			QByteArray getMessageTimeAsByteArray() const;
			static int getSizeOfMessageTimeInBytes();
			virtual std::string toString() const;
			virtual QString toQString() const;

			bool isNull() const;

			bool operator ==(MessageTime const& other) const;

			static MessageTime now();
			static MessageTime fromDatabase(qint64 msecs);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<MessageTime, true>;
#endif
		private:
			QDateTime messageTime;

			bool null;
		};

		utility::QtHashSizeType qHash(MessageTime const& key, utility::QtHashSizeType seed);
	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::MessageTime)

#endif // OPENMITTSU_PROTOCOL_MESSAGETIME_H_

