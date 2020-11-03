#ifndef OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
#define OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_

#include <QByteArray>

namespace openmittsu {
	namespace messages {

		class MessageFlags {
		public:
			MessageFlags();
			MessageFlags(uint8_t messageFlags);
			MessageFlags(bool pushMessage, bool noQueuing, bool noAckExpected, bool noDeliveryReceipt, bool messageHasAlreadyBeenDelivered, bool groupMessage, bool callMessage);
			MessageFlags(MessageFlags const& other);
			virtual ~MessageFlags();

			virtual uint8_t getFlags() const;

			virtual bool isPushMessage() const;
			virtual bool isNoQueueMessage() const;
			virtual bool isNoAckExpectedForMessage() const;
			virtual bool sendNoDeliveryReceiptForMessage() const;
			virtual bool isMessageHasAlreadyBeenDelivered() const;
			virtual bool isGroupMessage() const;
			virtual bool isCallMessage() const;
		private:
			bool m_pushMessage;
			bool m_noQueuing;
			bool m_noAckExpected;
			bool m_noDeliveryReceipt;
			bool m_messageHasAlreadyBeenDelivered;
			bool m_groupMessage;
			bool m_callMessage;

			static uint8_t boolsToByte(bool bitLsbPlus0, bool bitLsbPlus1, bool bitLsbPlus2, bool bitLsbPlus3, bool bitLsbPlus4, bool bitLsbPlus5, bool bitLsbPlus6, bool bitLsbPlus7);
			static bool byteToBool(uint8_t c, size_t position);
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
