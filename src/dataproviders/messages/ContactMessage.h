#ifndef OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/dataproviders/messages/ContactMessageType.h"
#include "src/dataproviders/messages/UserMessage.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ContactMessage : public virtual UserMessage {
			public:
				ContactMessage() {}
				virtual ~ContactMessage() {}

				virtual openmittsu::protocol::ContactId const& getContactId() const = 0;
				virtual ContactMessageType getMessageType() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGE_H_
