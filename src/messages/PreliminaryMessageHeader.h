#ifndef OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>

#include "src/messages/MessageFlags.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

namespace openmittsu {
	namespace messages {

		class PreliminaryMessageHeader {
		public:
			virtual ~PreliminaryMessageHeader();

			virtual openmittsu::protocol::MessageId const& getMessageId() const;
			virtual openmittsu::protocol::MessageTime const& getTime() const;

			virtual MessageFlags const& getFlags() const;
		protected:
			PreliminaryMessageHeader();
			PreliminaryMessageHeader(openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags);
			PreliminaryMessageHeader(PreliminaryMessageHeader const& other);
		private:
			openmittsu::protocol::MessageId const m_messageId;
			openmittsu::protocol::MessageTime const m_time;
			MessageFlags const m_flags;
		};

	}
}

#endif // OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEHEADER_H_
