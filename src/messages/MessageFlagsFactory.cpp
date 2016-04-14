#include "messages/MessageFlagsFactory.h"

MessageFlagsFactory::MessageFlagsFactory() {
	throw;
}

MessageFlagsFactory::MessageFlagsFactory(MessageFlagsFactory const& other) {
	throw;
}

MessageFlagsFactory::~MessageFlagsFactory() {
	// Intentionally left empty.
}

MessageFlags MessageFlagsFactory::createContactMessageFlags() {
	return MessageFlags(true, false, false, false, false);
}

MessageFlags MessageFlagsFactory::createGroupTextMessageFlags() {
	return MessageFlags(true, false, false, false, true);
}

MessageFlags MessageFlagsFactory::createGroupControlMessageFlags() {
	return MessageFlags(false, false, false, false, true);
}

MessageFlags MessageFlagsFactory::createTypingStatusMessageFlags() {
	return MessageFlags(false, true, true, false, false);
}

MessageFlags MessageFlagsFactory::createReceiptMessageFlags() {
	return MessageFlags(false, false, false, false, false);
}
