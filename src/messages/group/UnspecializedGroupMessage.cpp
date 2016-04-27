#include "messages/group/UnspecializedGroupMessage.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"

UnspecializedGroupMessage::UnspecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent) : Message(messageHeader, messageContent) {
	// Intentionally left empty.
}

UnspecializedGroupMessage::UnspecializedGroupMessage(PreliminaryGroupMessage const& message, ContactId const& sender, PushFromId const& pushFromId) : Message(FullMessageHeader(message.getPreliminaryMessageHeader(), sender, pushFromId), message.getPreliminaryMessageContent()->clone()) {
	// Intentionally left empty.
}

UnspecializedGroupMessage::UnspecializedGroupMessage(UnspecializedGroupMessage const& other) : Message(other.getMessageHeader(), other.getGroupMessageContent()->clone()) {
	// Intentionally left empty.
}

UnspecializedGroupMessage::~UnspecializedGroupMessage() {
	// Intentionally left empty.
}

GroupMessageContent const* UnspecializedGroupMessage::getGroupMessageContent() const {
	GroupMessageContent const * const gmc = dynamic_cast<GroupMessageContent const*>(getMessageContent());

	if (gmc == nullptr) {
		throw InternalErrorException() << "This UnspecializedGroupMessage does not contain a GroupMessageContent?!";
	}

	return gmc;
}

Message* UnspecializedGroupMessage::withNewMessageContent(MessageContent* messageContent) const {
	if (dynamic_cast<GroupMessageContent*>(messageContent) == nullptr) {
		throw IllegalArgumentException() << "Illegal Message content type, can not change from UnspecializedGroup- to Non-UnspecializedGroup message.";
	}
	return new UnspecializedGroupMessage(messageHeader, dynamic_cast<GroupMessageContent*>(messageContent));
}

