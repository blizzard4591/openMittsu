#ifndef OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_
#define OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_

#include "src/messages/MessageWithPayload.h"
#include "src/messages/Message.h"

#include <QByteArray>
#include <QMutex>

#include <memory>

namespace openmittsu {
	namespace messages {

		class IncomingMessagesParser {
		public:
			static std::shared_ptr<Message> parseMessageWithPayloadToMessage(MessageWithPayload const& messageWithPayload);
		};

	}
}

#endif // OPENMITTSU_MESSAGES_INCOMINGMESSAGESPARSER_H_
