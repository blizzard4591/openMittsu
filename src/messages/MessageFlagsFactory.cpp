#include "src/messages/MessageFlagsFactory.h"

namespace openmittsu {
	namespace messages {

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
			return MessageFlags(true, false, false, false, false, false);
		}

		MessageFlags MessageFlagsFactory::createGroupTextMessageFlags() {
			return MessageFlags(true, false, false, false, true, false);
		}

		MessageFlags MessageFlagsFactory::createGroupControlMessageFlags() {
			return MessageFlags(false, false, false, false, true, false);
		}

		MessageFlags MessageFlagsFactory::createTypingStatusMessageFlags() {
			return MessageFlags(false, true, true, false, false, false);
		}

		MessageFlags MessageFlagsFactory::createReceiptMessageFlags() {
			return MessageFlags(false, false, false, false, false, false);
		}

	}
}
