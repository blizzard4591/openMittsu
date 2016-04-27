#include "messages/contact/ContactMessage.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"

ContactMessage::ContactMessage(FullMessageHeader const& messageHeader, ContactMessageContent* messageContent) : Message(messageHeader, messageContent) {
	// Intentionally left empty.
}

ContactMessage::ContactMessage(ContactMessage const& other) : Message(other.getMessageHeader(), other.getContactMessageContent()->clone()) {
	// Intentionally left empty.
}

ContactMessage::~ContactMessage() {
	// Intentionally left empty.
}

ContactMessageContent const* ContactMessage::getContactMessageContent() const {
	ContactMessageContent const * const cmc = dynamic_cast<ContactMessageContent const*>(getMessageContent());
	if (cmc == nullptr) {
		throw InternalErrorException() << "This ContactMessage does not contain a ContactMessageContent?!";
	}

	return cmc;
}

Message* ContactMessage::withNewMessageContent(MessageContent* messageContent) const {
	if (dynamic_cast<ContactMessageContent*>(messageContent) == nullptr) {
		throw IllegalArgumentException() << "Illegal Message content type, can not change from Contact- to Non-contact message.";
	}
	return new ContactMessage(messageHeader, dynamic_cast<ContactMessageContent*>(messageContent));
}
