#include "messages/contact/UserTypingMessageContent.h"

#include "exceptions/ProtocolErrorException.h"
#include "messages/MessageContentRegistry.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/HexChar.h"

// Register this MessageContent with the MessageContentRegistry
bool UserTypingMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_TYPINGNOTIFICATION, new TypedMessageContentFactory<UserTypingMessageContent>());


UserTypingMessageContent::UserTypingMessageContent() : ContactMessageContent(), isTyping(false) {
	// Only accessible and used by the MessageContentFactory.
}

UserTypingMessageContent::UserTypingMessageContent(bool isTyping) : ContactMessageContent(), isTyping(isTyping) {
	// Intentionally left empty.
}

UserTypingMessageContent::~UserTypingMessageContent() {
	// Intentionally left empty.
}

bool UserTypingMessageContent::isUserTyping() const {
	return isTyping;
}

ContactMessageContent* UserTypingMessageContent::clone() const {
	return new UserTypingMessageContent(isTyping);
}

MessageContent* UserTypingMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_TYPINGNOTIFICATION, 2, payload, true);

	bool typing = false;
	char const typeChar = payload.at(1);
	if (typeChar == (PROTO_TYPINGNOTIFICATION_TYPE_STOPPED)) {
		typing = false;
	} else if (typeChar == (PROTO_TYPINGNOTIFICATION_TYPE_TYPING)) {
		typing = true;
	} else {
		throw ProtocolErrorException() << "Received a typing status indicator of unknown type 0x" << HexChar(typeChar) << ".";
	}

	return new UserTypingMessageContent(typing);
}

QByteArray UserTypingMessageContent::toPacketPayload() const {
	QByteArray result(2, 0x00);

	result[0] = PROTO_MESSAGE_SIGNATURE_TYPINGNOTIFICATION;
	if (isTyping) {
		result[1] = PROTO_TYPINGNOTIFICATION_TYPE_TYPING;
	} else {
		result[1] = PROTO_TYPINGNOTIFICATION_TYPE_STOPPED;
	}
	
	return result;
}
