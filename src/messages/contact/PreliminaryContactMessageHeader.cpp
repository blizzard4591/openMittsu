#include "messages/contact/PreliminaryContactMessageHeader.h"

PreliminaryContactMessageHeader::PreliminaryContactMessageHeader() : PreliminaryMessageHeader(), receiverId(0) {
	// This constructor is disabled and should not be used.
	throw;
}

PreliminaryContactMessageHeader::PreliminaryContactMessageHeader(ContactId const& receiver, MessageId const& messageId, MessageFlags const& messageFlags) : PreliminaryMessageHeader(messageId, messageFlags), receiverId(receiver) {
	// Intentionally left empty.
}

PreliminaryContactMessageHeader::PreliminaryContactMessageHeader(PreliminaryContactMessageHeader const& other) : PreliminaryMessageHeader(other), receiverId(other.receiverId) {
	// Intentionally left empty.
}

PreliminaryContactMessageHeader::~PreliminaryContactMessageHeader() {
	// Intentionally left empty.
}

ContactId const& PreliminaryContactMessageHeader::getReceiver() const {
	return receiverId;
}

PreliminaryContactMessageHeader* PreliminaryContactMessageHeader::clone() const {
	return new PreliminaryContactMessageHeader(*this);
}
