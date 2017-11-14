#include "src/messages/contact/PreliminaryContactMessageHeader.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			PreliminaryContactMessageHeader::PreliminaryContactMessageHeader() : PreliminaryMessageHeader(), receiverId(0) {
				// This constructor is disabled and should not be used.
				throw;
			}

			PreliminaryContactMessageHeader::PreliminaryContactMessageHeader(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags) : PreliminaryMessageHeader(messageId, time, messageFlags), receiverId(receiver) {
				// Intentionally left empty.
			}

			PreliminaryContactMessageHeader::PreliminaryContactMessageHeader(PreliminaryContactMessageHeader const& other) : PreliminaryMessageHeader(other), receiverId(other.receiverId) {
				// Intentionally left empty.
			}

			PreliminaryContactMessageHeader::~PreliminaryContactMessageHeader() {
				// Intentionally left empty.
			}

			openmittsu::protocol::ContactId const& PreliminaryContactMessageHeader::getReceiver() const {
				return receiverId;
			}

			PreliminaryContactMessageHeader* PreliminaryContactMessageHeader::clone() const {
				return new PreliminaryContactMessageHeader(*this);
			}

		}
	}
}
