#include "messages/PreliminaryMessageHeader.h"

PreliminaryMessageHeader::PreliminaryMessageHeader() : messageId(0), time(0), flags(false, false, false, false, false) {
	// This constructor is disabled and should not be used.
	throw;
}

PreliminaryMessageHeader::PreliminaryMessageHeader(MessageId const& messageId, MessageFlags const& messageFlags) : messageId(messageId), time(MessageTime::now()), flags(messageFlags) {
	// Intentionally left empty.
}

PreliminaryMessageHeader::PreliminaryMessageHeader(PreliminaryMessageHeader const& other) : messageId(other.messageId), time(other.time), flags(other.flags) {
	// Intentionally left empty.
}

PreliminaryMessageHeader::~PreliminaryMessageHeader() {
	// Intentionally left empty.
}

MessageId const& PreliminaryMessageHeader::getMessageId() const {
	return messageId;
}

MessageTime const& PreliminaryMessageHeader::getTime() const {
	return time;
}

MessageFlags const& PreliminaryMessageHeader::getFlags() const {
	return flags;
}
