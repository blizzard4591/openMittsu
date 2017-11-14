#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class Message {
			public:
				Message() {}
				virtual ~Message() {}

				virtual openmittsu::protocol::ContactId getSender() const = 0;
				virtual openmittsu::protocol::MessageId const& getMessageId() const = 0;

				virtual bool isMessageFromUs() const = 0;

				/** Encodes whether this is an outgoing or incoming message. */
				virtual bool isOutbox() const = 0;

				virtual openmittsu::protocol::MessageTime getCreatedAt() const = 0;
				virtual openmittsu::protocol::MessageTime getSentAt() const = 0;
				virtual openmittsu::protocol::MessageTime getModifiedAt() const = 0;
	
				/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
				virtual bool isQueued() const = 0;

				/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
				virtual bool isSent() const = 0;

				virtual void setIsQueued(bool isQueuedStatus) = 0;
				virtual void setIsSent() = 0;

				virtual QString getUid() const = 0;
			};


		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGE_H_
