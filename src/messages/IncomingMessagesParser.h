#ifndef OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_
#define OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_

#include "messages/MessageWithPayload.h"
#include "messages/Message.h"

#include <QByteArray>
#include <QMutex>

#include <memory>

class IncomingMessagesParser {
public:
	static std::shared_ptr<Message> parseMessageWithPayloadToMessage(MessageWithPayload const& messageWithPayload);
};

#endif // OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_
