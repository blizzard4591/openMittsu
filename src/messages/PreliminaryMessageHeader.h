#ifndef OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>

#include "messages/MessageFlags.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/MessageTime.h"

class PreliminaryMessageHeader {
public:
	virtual ~PreliminaryMessageHeader();

	virtual MessageId const& getMessageId() const;
	virtual MessageTime const& getTime() const;

	virtual MessageFlags const& getFlags() const;
protected:
	PreliminaryMessageHeader();
	PreliminaryMessageHeader(MessageId const& messageId, MessageFlags const& messageFlags);
	PreliminaryMessageHeader(PreliminaryMessageHeader const& other);
private:
	MessageId const messageId;
	MessageTime const time;
	MessageFlags const flags;
};

#endif // OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_
