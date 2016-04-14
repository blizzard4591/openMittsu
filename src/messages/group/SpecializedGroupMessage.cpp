#include "messages/group/SpecializedGroupMessage.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"

#include "messages/MessageFlagsFactory.h"

SpecializedGroupMessage::SpecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent) : Message(messageHeader, messageContent) {
	// Intentionally left empty.
}

SpecializedGroupMessage::SpecializedGroupMessage(UnspecializedGroupMessage const& message, ContactId const& receiver, MessageId const& uniqueMessageId) : Message(FullMessageHeader(message.getMessageHeader(), receiver, uniqueMessageId), message.getGroupMessageContent()->clone()) {
	// Intentionally left empty.
}

SpecializedGroupMessage::SpecializedGroupMessage(SpecializedGroupMessage const& other) : Message(other.getMessageHeader(), other.getGroupMessageContent()->clone()) {
	// Intentionally left empty.
}

SpecializedGroupMessage::~SpecializedGroupMessage() {
	// Intentionally left empty.
}

GroupMessageContent const* SpecializedGroupMessage::getGroupMessageContent() const {
	GroupMessageContent const * const gmc = dynamic_cast<GroupMessageContent const*>(getMessageContent());

	if (gmc == nullptr) {
		throw InternalErrorException() << "This SpecializedGroupMessage does not contain a GroupMessageContent?!";
	}

	return gmc;
}

Message* SpecializedGroupMessage::withNewMessageContent(MessageContent* messageContent) const {
	if (dynamic_cast<GroupMessageContent*>(messageContent) == nullptr) {
		throw IllegalArgumentException() << "Illegal Message content type, can not change from SpecializedGroup- to Non-SpecializedGroup message.";
	}
	return new SpecializedGroupMessage(messageHeader, dynamic_cast<GroupMessageContent*>(messageContent));
}
