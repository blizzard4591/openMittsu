#ifndef OPENMITTSU_PROTOCOL_MESSAGEID_H_
#define OPENMITTSU_PROTOCOL_MESSAGEID_H_

#include "protocol/Id.h"
#include <cstdint>
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>
#include <QVector>

class MessageId : public Id {
public:
	explicit MessageId(quint64 messageId);
	MessageId(QByteArray const& messageIdBytes);
	MessageId(MessageId const& other);
	virtual ~MessageId();

	quint64 getMessageId() const;
	QByteArray getMessageIdAsByteArray() const;
	static int getSizeOfMessageIdInBytes();
	virtual std::string toString() const override;
	virtual QString toQString() const override;

	bool operator ==(MessageId const& other) const;

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<MessageId, true>;
	friend class QVector<MessageId>;
private:
	quint64 messageId;

	MessageId();
};

uint qHash(MessageId const& key, uint seed);

Q_DECLARE_METATYPE(MessageId)

#endif // #define OPENMITTSU_PROTOCOL_MESSAGEID_H_

