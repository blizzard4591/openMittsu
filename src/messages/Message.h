#ifndef OPENMITTSU_MESSAGES_MESSAGE_H_
#define OPENMITTSU_MESSAGES_MESSAGE_H_

#include "messages/FullMessageHeader.h"
#include "messages/MessageContent.h"

#include <memory>

class IncomingMessagesParser;

class Message {
public:
	virtual ~Message();

	virtual FullMessageHeader const& getMessageHeader() const;
	virtual MessageContent const* getMessageContent() const;

	virtual Message* withNewMessageContent(MessageContent* messageContent) const;

	friend class IncomingMessagesParser;
protected:
	Message(FullMessageHeader const& messageHeader, MessageContent* messageContent);

	FullMessageHeader const messageHeader;
	std::unique_ptr<MessageContent const> const messageContentStorage;
};

#endif // OPENMITTSU_MESSAGES_MESSAGE_H_
