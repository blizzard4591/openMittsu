#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYCONTACTMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYCONTACTMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/dataproviders/messages/ContactMessageType.h"
#include "src/dataproviders/messages/ReadonlyUserMessage.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ReadonlyContactMessage : public virtual ReadonlyUserMessage {
			public:
				ReadonlyContactMessage() {}
				virtual ~ReadonlyContactMessage() {}

				virtual openmittsu::protocol::ContactId const& getContactId() const = 0;
				virtual ContactMessageType const& getMessageType() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYCONTACTMESSAGE_H_
