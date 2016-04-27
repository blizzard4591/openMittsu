#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_

#include "messages/Message.h"
#include "messages/FullMessageHeader.h"
#include "messages/contact/ContactMessageContent.h"
#include "messages/contact/PreliminaryContactMessage.h"
#include "protocol/ContactId.h"

class ContactMessage : public Message {
public:
	ContactMessage(FullMessageHeader const& messageHeader, ContactMessageContent* messageContent);
	ContactMessage(ContactMessage const& other);
	virtual ~ContactMessage();

	virtual ContactMessageContent const* getContactMessageContent() const;
	virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
};

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGE_H_
