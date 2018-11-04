#include "src/messages/MessageFlags.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayToHexString.h"

namespace openmittsu {
	namespace messages {

		MessageFlags::MessageFlags() : m_pushMessage(false), m_noQueuing(false), m_noAckExpected(false), m_messageHasAlreadyBeenDelivered(false), m_groupMessage(false), m_callMessage(false) {
			// Intentionally left empty.
		}

		MessageFlags::MessageFlags(char messageFlags) : m_pushMessage(byteToBool(messageFlags, 0)), m_noQueuing(byteToBool(messageFlags, 1)), m_noAckExpected(byteToBool(messageFlags, 2)), m_messageHasAlreadyBeenDelivered(byteToBool(messageFlags, 3)), m_groupMessage(byteToBool(messageFlags, 4)), m_callMessage(byteToBool(messageFlags, 5)) {
			// Check if only the lowest 6 bits are used, if at all.
			if ((messageFlags & (~0x3F)) != 0x00) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Unknown Message Flag Bits are set: " << openmittsu::utility::ByteArrayToHexString::charToHexString(messageFlags).toStdString();
			}
		}

		MessageFlags::MessageFlags(bool pushMessage, bool noQueuing, bool noAckExpected, bool messageHasAlreadyBeenDelivered, bool groupMessage, bool callMessage) : m_pushMessage(pushMessage), m_noQueuing(noQueuing), m_noAckExpected(noAckExpected), m_messageHasAlreadyBeenDelivered(messageHasAlreadyBeenDelivered), m_groupMessage(groupMessage), m_callMessage(callMessage) {
			// Intentionally left empty.
		}

		MessageFlags::MessageFlags(MessageFlags const& other) : m_pushMessage(other.m_pushMessage), m_noQueuing(other.m_noQueuing), m_noAckExpected(other.m_noAckExpected), m_messageHasAlreadyBeenDelivered(other.m_messageHasAlreadyBeenDelivered), m_groupMessage(other.m_groupMessage), m_callMessage(other.m_callMessage) {
			// Intentionally left empty.
		}

		MessageFlags::~MessageFlags() {
			// Intentionally left empty.
		}

		char MessageFlags::getFlags() const {
			return boolsToByte(m_pushMessage, m_noQueuing, m_noAckExpected, m_messageHasAlreadyBeenDelivered, m_groupMessage, m_callMessage, false, false);
		}

		bool MessageFlags::isPushMessage() const {
			return m_pushMessage;
		}

		bool MessageFlags::isNoQueueMessage() const {
			return m_noQueuing;
		}

		bool MessageFlags::isNoAckExpectedForMessage() const {
			return m_noAckExpected;
		}

		bool MessageFlags::isMessageHasAlreadyBeenDelivered() const {
			return m_messageHasAlreadyBeenDelivered;
		}

		bool MessageFlags::isGroupMessage() const {
			return m_groupMessage;
		}

		bool MessageFlags::isCallMessage() const {
			return m_callMessage;
		}

		char MessageFlags::boolsToByte(bool bitLsbPlus0, bool bitLsbPlus1, bool bitLsbPlus2, bool bitLsbPlus3, bool bitLsbPlus4, bool bitLsbPlus5, bool bitLsbPlus6, bool bitLsbPlus7) {
			char c = 0x00;

			// LSB + 0
			if (bitLsbPlus0) {
				c |= (1 << 0);
			}
			// LSB + 1
			if (bitLsbPlus1) {
				c |= (1 << 1);
			}
			// LSB + 2
			if (bitLsbPlus2) {
				c |= (1 << 2);
			}
			// LSB + 3
			if (bitLsbPlus3) {
				c |= (1 << 3);
			}
			// LSB + 4
			if (bitLsbPlus4) {
				c |= (1 << 4);
			}
			// LSB + 5
			if (bitLsbPlus5) {
				c |= (1 << 5);
			}
			// LSB + 6
			if (bitLsbPlus6) {
				c |= (1 << 6);
			}
			// LSB + 7
			if (bitLsbPlus7) {
				c |= (1 << 7);
			}

			return c;
		}

		bool MessageFlags::byteToBool(char c, size_t position) {
			if (position > 7) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Position is out of bounds, only values from 0 to 7 are allowed. Given: " << position;
			}

			return ((c >> position) & 0x01) == 0x01;
		}

	}
}
