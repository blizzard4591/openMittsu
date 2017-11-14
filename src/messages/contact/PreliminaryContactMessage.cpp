#include "src/messages/contact/PreliminaryContactMessage.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			PreliminaryContactMessage::PreliminaryContactMessage() : preliminaryMessageHeader(nullptr), messageContent(nullptr) {
				// This constructor is disabled and should not be used.
				throw;
			}

			PreliminaryContactMessage::PreliminaryContactMessage(PreliminaryContactMessageHeader* preliminaryMessageHeader, ContactMessageContent* messageContent) : preliminaryMessageHeader(preliminaryMessageHeader), messageContent(messageContent) {
				// Intentionally left empty.
			}

			PreliminaryContactMessage::PreliminaryContactMessage(PreliminaryContactMessage const& other) : preliminaryMessageHeader(other.preliminaryMessageHeader->clone()), messageContent(other.messageContent->clone()) {
				// Intentionally left empty.
			}

			PreliminaryContactMessage::~PreliminaryContactMessage() {
				// Intentionally left empty.
			}

			PreliminaryContactMessageHeader const* PreliminaryContactMessage::getPreliminaryMessageHeader() const {
				return preliminaryMessageHeader.get();
			}

			ContactMessageContent const* PreliminaryContactMessage::getPreliminaryMessageContent() const {
				return messageContent.get();
			}

		}
	}
}
