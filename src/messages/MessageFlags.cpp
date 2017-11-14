#include "src/messages/MessageFlags.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayToHexString.h"

namespace openmittsu {
	namespace messages {

		MessageFlags::MessageFlags() : pushMessage(false), noQueuing(false), noAckExpected(false), messageHasAlreadyBeenDelivered(false), groupMessage(false) {
			// Intentionally left empty.
		}

		MessageFlags::MessageFlags(char messageFlags) : pushMessage(byteToBool(messageFlags, 0)), noQueuing(byteToBool(messageFlags, 1)), noAckExpected(byteToBool(messageFlags, 2)), messageHasAlreadyBeenDelivered(byteToBool(messageFlags, 3)), groupMessage(byteToBool(messageFlags, 4)) {
			// Check if only the lowest 5 bits are used, if at all.
			if ((messageFlags & (~0x1F)) != 0x00) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Unknown Message Flag Bits are set: " << ByteArrayToHexString::charToHexString(messageFlags).toStdString();
			}
		}

		MessageFlags::MessageFlags(bool pushMessage, bool noQueuing, bool noAckExpected, bool messageHasAlreadyBeenDelivered, bool groupMessage) : pushMessage(pushMessage), noQueuing(noQueuing), noAckExpected(noAckExpected), messageHasAlreadyBeenDelivered(messageHasAlreadyBeenDelivered), groupMessage(groupMessage) {
			// Intentionally left empty.
		}

		MessageFlags::MessageFlags(MessageFlags const& other) : pushMessage(other.pushMessage), noQueuing(other.noQueuing), noAckExpected(other.noAckExpected), messageHasAlreadyBeenDelivered(other.messageHasAlreadyBeenDelivered), groupMessage(other.groupMessage) {
			// Intentionally left empty.
		}

		MessageFlags::~MessageFlags() {
			// Intentionally left empty.
		}

		char MessageFlags::getFlags() const {
			return boolsToByte(pushMessage, noQueuing, noAckExpected, messageHasAlreadyBeenDelivered, groupMessage, false, false, false);
		}

		bool MessageFlags::isPushMessage() const {
			return pushMessage;
		}

		bool MessageFlags::isNoQueueMessage() const {
			return noQueuing;
		}

		bool MessageFlags::isNoAckExpectedForMessage() const {
			return noAckExpected;
		}

		bool MessageFlags::isMessageHasAlreadyBeenDelivered() const {
			return messageHasAlreadyBeenDelivered;
		}

		bool MessageFlags::isGroupMessage() const {
			return groupMessage;
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
