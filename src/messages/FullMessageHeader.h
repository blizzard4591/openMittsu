#ifndef OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QMetaType>
#include <QSharedPointer>

#include "messages/MessageFlags.h"

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/MessageTime.h"
#include "protocol/PushFromId.h"

class PreliminaryContactMessageHeader;
class PreliminaryGroupMessageHeader;

class FullMessageHeader {
public:
	FullMessageHeader(ContactId const& receiverId, MessageTime const& time, ContactId const& sender, MessageId const& messageId, MessageFlags const& flags);
	FullMessageHeader(PreliminaryContactMessageHeader const* preliminaryHeader, ContactId const& sender);
	FullMessageHeader(PreliminaryGroupMessageHeader const* preliminaryHeader, ContactId const& sender);
	FullMessageHeader(FullMessageHeader const& messageHeader, ContactId const& replacementReceiver, MessageId const& replacementMessageId);
	FullMessageHeader(FullMessageHeader const& other);
	virtual ~FullMessageHeader();

	static FullMessageHeader fromHeaderData(QByteArray const& headerData);
	static int getFullMessageHeaderSize();

	virtual ContactId const& getSender() const;
	virtual ContactId const& getReceiver() const;
	virtual MessageId const& getMessageId() const;
	virtual MessageTime const& getTime() const;
	virtual MessageFlags const& getFlags() const;
	virtual PushFromId const& getPushFromName() const;

	virtual QByteArray toPacket() const;

	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<FullMessageHeader, true>;
private:
	ContactId const sender;
	ContactId const receiver;
	MessageId const messageId;
	MessageTime const time;
	MessageFlags const flags;
	PushFromId const pushFromName;

	FullMessageHeader();
};

Q_DECLARE_METATYPE(FullMessageHeader)
Q_DECLARE_METATYPE(QSharedPointer<FullMessageHeader const>)

#endif // OPENMITTSU_MESSAGES_FULLMESSAGEHEADER_H_
