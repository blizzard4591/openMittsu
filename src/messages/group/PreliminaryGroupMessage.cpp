#include "src/messages/group/PreliminaryGroupMessage.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			PreliminaryGroupMessage::PreliminaryGroupMessage() : m_preliminaryMessageHeader(nullptr), m_messageContent(nullptr), m_recipients() {
				// This constructor is disabled and should not be used.
				throw;
			}

			PreliminaryGroupMessage::PreliminaryGroupMessage(PreliminaryGroupMessageHeader* preliminaryMessageHeader, GroupMessageContent* messageContent, QSet<openmittsu::protocol::ContactId> const& recipients) : m_preliminaryMessageHeader(preliminaryMessageHeader), m_messageContent(messageContent), m_recipients(recipients) {
				// Intentionally left empty.
			}

			PreliminaryGroupMessage::PreliminaryGroupMessage(PreliminaryGroupMessage const& other) : m_preliminaryMessageHeader(other.m_preliminaryMessageHeader->clone()), m_messageContent(other.m_messageContent->clone()), m_recipients(other.m_recipients) {
				// Intentionally left empty.
			}

			PreliminaryGroupMessage::~PreliminaryGroupMessage() {
				// Intentionally left empty.
			}

			PreliminaryGroupMessageHeader const* PreliminaryGroupMessage::getPreliminaryMessageHeader() const {
				return m_preliminaryMessageHeader.get();
			}

			GroupMessageContent const* PreliminaryGroupMessage::getPreliminaryMessageContent() const {
				return m_messageContent.get();
			}

			QSet<openmittsu::protocol::ContactId> const& PreliminaryGroupMessage::getRecipients() const {
				return m_recipients;
			}

		}
	}
}
