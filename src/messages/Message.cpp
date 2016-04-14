#include "messages/Message.h"

#include "exceptions/IllegalArgumentException.h"

Message::Message(FullMessageHeader const& messageHeader, MessageContent* messageContent) : messageHeader(messageHeader), messageContentStorage(messageContent) {
	if (messageContent == nullptr) {
		throw IllegalArgumentException() << "A Message may not contain a null message content.";
	}
}

Message::~Message() {
	// Intentionally left empty.
}

FullMessageHeader const& Message::getMessageHeader() const {
	return messageHeader;
}

MessageContent const* Message::getMessageContent() const {
	return messageContentStorage.get();
}

Message* Message::withNewMessageContent(MessageContent* messageContent) const {
	return new Message(messageHeader, messageContent);
}
