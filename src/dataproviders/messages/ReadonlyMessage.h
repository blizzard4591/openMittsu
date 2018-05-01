#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ReadonlyMessage {
			public:
				ReadonlyMessage() {}
				virtual ~ReadonlyMessage() {}

				virtual openmittsu::protocol::ContactId const& getSender() const = 0;
				virtual openmittsu::protocol::MessageId const& getMessageId() const = 0;

				/** Encodes whether this is an outgoing or incoming message. */
				virtual bool isMessageFromUs() const = 0;

				virtual openmittsu::protocol::MessageTime const& getCreatedAt() const = 0;
				virtual openmittsu::protocol::MessageTime const& getSentAt() const = 0;
				virtual openmittsu::protocol::MessageTime const& getModifiedAt() const = 0;
	
				/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
				virtual bool isQueued() const = 0;

				/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
				virtual bool isSent() const = 0;

				virtual QString const& getUid() const = 0;
			};


		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYMESSAGE_H_
