#ifndef OPENMITTSU_PROTOCOL_MESSAGETIME_H_
#define OPENMITTSU_PROTOCOL_MESSAGETIME_H_

#include <cstdint>
#include <QtGlobal>
#include <QDateTime>
#include <QByteArray>
#include <QMetaType>

class MessageTime {
public:
	explicit MessageTime(QDateTime const& messageTime);
	explicit MessageTime(QByteArray const& messageTimeBytes);
	MessageTime(MessageTime const& other);
	virtual ~MessageTime();

	quint64 getMessageTime() const;
	QDateTime const& getTime() const;
	QByteArray getMessageTimeAsByteArray() const;
	static int getSizeOfMessageTimeInBytes();
	virtual std::string toString() const;
	virtual QString toQString() const;

	bool operator ==(MessageTime const& other) const;

	static MessageTime now();

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<MessageTime, true>;
private:
	QDateTime messageTime;

	MessageTime();
};

uint qHash(MessageTime const& key, uint seed);

Q_DECLARE_METATYPE(MessageTime)

#endif // OPENMITTSU_PROTOCOL_MESSAGETIME_H_

