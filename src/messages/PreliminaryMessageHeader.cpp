#include "src/messages/PreliminaryMessageHeader.h"

namespace openmittsu {
	namespace messages {

		PreliminaryMessageHeader::PreliminaryMessageHeader() : m_messageId(0), m_time(0), m_flags(false, false, false, false, false) {
			// This constructor is disabled and should not be used.
			throw;
		}

		PreliminaryMessageHeader::PreliminaryMessageHeader(openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags) : m_messageId(messageId), m_time(time), m_flags(messageFlags) {
			// Intentionally left empty.
		}

		PreliminaryMessageHeader::PreliminaryMessageHeader(PreliminaryMessageHeader const& other) : m_messageId(other.m_messageId), m_time(other.m_time), m_flags(other.m_flags) {
			// Intentionally left empty.
		}

		PreliminaryMessageHeader::~PreliminaryMessageHeader() {
			// Intentionally left empty.
		}

		openmittsu::protocol::MessageId const& PreliminaryMessageHeader::getMessageId() const {
			return m_messageId;
		}

		openmittsu::protocol::MessageTime const& PreliminaryMessageHeader::getTime() const {
			return m_time;
		}

		MessageFlags const& PreliminaryMessageHeader::getFlags() const {
			return m_flags;
		}

	}
}
