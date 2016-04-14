#include "messages/IncomingMessagesParser.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "exceptions/ProtocolErrorException.h"
#include "messages/MessageContentRegistry.h"
#include "messages/contact/ContactMessage.h"
#include "messages/contact/ContactMessageContent.h"
#include "messages/group/SpecializedGroupMessage.h"
#include "messages/group/GroupMessageContent.h"
#include "utility/HexChar.h"

std::shared_ptr<Message> IncomingMessagesParser::parseMessageWithPayloadToMessage(MessageWithPayload const& messageWithPayload) {
	MessageContentRegistry& messageContentRegistry = MessageContentRegistry::getInstance();

	if (messageWithPayload.getPayload().size() < 1) {
		throw IllegalArgumentException() << "The payload of a message may not be empty.";
	}

	char const signatureByte = messageWithPayload.getPayload().at(0);

	MessageContentFactory* messageContentFactory = messageContentRegistry.getMessageContentFactoryForSignatureByte(signatureByte);
	if (messageContentFactory == nullptr) {
		throw ProtocolErrorException() << "Could not match message payload with signature byte 0x" << HexChar(signatureByte) << " to any existing MessageContents. Payload: " << QString(messageWithPayload.getPayload().toHex()).toStdString();
	}

	MessageContent* const messageContent = messageContentFactory->createFromPacketPayload(messageWithPayload.getMessageHeader(), messageWithPayload.getPayload());

	if (dynamic_cast<ContactMessageContent*>(messageContent) != nullptr) {
		return std::make_shared<ContactMessage>(messageWithPayload.getMessageHeader(), dynamic_cast<ContactMessageContent*>(messageContent));
	} else if (dynamic_cast<GroupMessageContent*>(messageContent) != nullptr) {
		return std::make_shared<SpecializedGroupMessage>(messageWithPayload.getMessageHeader(), dynamic_cast<GroupMessageContent*>(messageContent));
	} else {
		throw InternalErrorException() << "Incoming Message could not be parsed, the message content is of unknown type: " << QString(messageWithPayload.getPayload().toHex()).toStdString();
	}
}
