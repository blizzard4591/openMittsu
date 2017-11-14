#include "src/protocol/ContactIdWithMessageId.h"

namespace openmittsu {
	namespace protocol {

		ContactIdWithMessageId::ContactIdWithMessageId() : contact(0), messageId(0) {
			// Used by the QMetaType system
		}

		ContactIdWithMessageId::ContactIdWithMessageId(ContactId const& contact, MessageId const& messageId) : contact(contact), messageId(messageId) {
			// Intentionally left empty.
		}

		ContactIdWithMessageId::ContactIdWithMessageId(ContactIdWithMessageId const& other) : contact(other.contact), messageId(other.messageId) {
			// Intentionally left empty.
		}

		ContactIdWithMessageId::~ContactIdWithMessageId() {
			// Intentionally left empty.
		}

		MessageId const& ContactIdWithMessageId::getMessageId() const {
			return messageId;
		}

		ContactId const& ContactIdWithMessageId::getContactId() const {
			return contact;
		}

		bool ContactIdWithMessageId::operator ==(ContactIdWithMessageId const& other) const {
			return (contact == other.contact) && (messageId == other.messageId);
		}

		uint qHash(ContactIdWithMessageId const& key, uint seed) {
			return qHash(key.getMessageId(), seed) ^ qHash(key.getContactId(), seed);
		}
	}
}
