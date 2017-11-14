#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_

#include "src/messages/Message.h"
#include "src/messages/FullMessageHeader.h"
#include "src/messages/contact/ContactMessageContent.h"
#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/protocol/ContactId.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactMessage : public openmittsu::messages::Message {
			public:
				ContactMessage(FullMessageHeader const& messageHeader, ContactMessageContent* messageContent);
				ContactMessage(ContactMessage const& other);
				virtual ~ContactMessage();

				virtual ContactMessageContent const* getContactMessageContent() const;
				virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_
