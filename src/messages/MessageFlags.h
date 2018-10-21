#ifndef OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
#define OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_

#include <QByteArray>

namespace openmittsu {
	namespace messages {

		class MessageFlags {
		public:
			MessageFlags();
			MessageFlags(char messageFlags);
			MessageFlags(bool pushMessage, bool noQueuing, bool noAckExpected, bool messageHasAlreadyBeenDelivered, bool groupMessage, bool callMessage);
			MessageFlags(MessageFlags const& other);
			virtual ~MessageFlags();

			virtual char getFlags() const;

			virtual bool isPushMessage() const;
			virtual bool isNoQueueMessage() const;
			virtual bool isNoAckExpectedForMessage() const;
			virtual bool isMessageHasAlreadyBeenDelivered() const;
			virtual bool isGroupMessage() const;
			virtual bool isCallMessage() const;
		private:
			bool const m_pushMessage;
			bool const m_noQueuing;
			bool const m_noAckExpected;
			bool const m_messageHasAlreadyBeenDelivered;
			bool const m_groupMessage;
			bool const m_callMessage;

			static char boolsToByte(bool bitLsbPlus0, bool bitLsbPlus1, bool bitLsbPlus2, bool bitLsbPlus3, bool bitLsbPlus4, bool bitLsbPlus5, bool bitLsbPlus6, bool bitLsbPlus7);
			static bool byteToBool(char c, size_t position);
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGEFLAGS_H_
