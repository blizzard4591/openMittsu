#include "src/messages/IncomingMessagesParser.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/ProtocolErrorException.h"
#include "src/messages/MessageContentFactory.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/contact/ContactMessage.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/messages/group/SpecializedGroupMessage.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/utility/HexChar.h"

namespace openmittsu {
	namespace messages {

		std::shared_ptr<Message> IncomingMessagesParser::parseMessageWithPayloadToMessage(MessageWithPayload const& messageWithPayload) {
			MessageContentRegistry& messageContentRegistry = MessageContentRegistry::getInstance();

			if (messageWithPayload.getPayload().size() < 1) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The payload of a message may not be empty.";
			}

			char const signatureByte = messageWithPayload.getPayload().at(0);

			MessageContentFactory* messageContentFactory = messageContentRegistry.getMessageContentFactoryForSignatureByte(signatureByte);
			if (messageContentFactory == nullptr) {
				throw openmittsu::exceptions::ProtocolErrorException() << "Could not match message payload with signature byte 0x" << openmittsu::utility::HexChar(signatureByte) << " to any existing MessageContents. Payload: " << QString(messageWithPayload.getPayload().toHex()).toStdString();
			}

			MessageContent* const messageContent = messageContentFactory->createFromPacketPayload(messageWithPayload.getMessageHeader(), messageWithPayload.getPayload());

			if (dynamic_cast<contact::ContactMessageContent*>(messageContent) != nullptr) {
				return std::make_shared<contact::ContactMessage>(messageWithPayload.getMessageHeader(), dynamic_cast<contact::ContactMessageContent*>(messageContent));
			} else if (dynamic_cast<group::GroupMessageContent*>(messageContent) != nullptr) {
				return std::make_shared<group::SpecializedGroupMessage>(messageWithPayload.getMessageHeader(), dynamic_cast<group::GroupMessageContent*>(messageContent));
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Incoming Message could not be parsed, the message content is of unknown type: " << QString(messageWithPayload.getPayload().toHex()).toStdString();
			}
		}

	}
}
