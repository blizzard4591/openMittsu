#ifndef OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "messages/PreliminaryMessageHeader.h"

class PreliminaryContactMessageHeader : public PreliminaryMessageHeader {
public:
	PreliminaryContactMessageHeader(ContactId const& receiver, MessageId const& messageId, MessageFlags const& messageFlags);
	PreliminaryContactMessageHeader(PreliminaryContactMessageHeader const& other);
	virtual ~PreliminaryContactMessageHeader();

	virtual ContactId const& getReceiver() const;

	virtual PreliminaryContactMessageHeader* clone() const;
private:
	ContactId const receiverId;

	// Disable the default constructor
	PreliminaryContactMessageHeader();
};

#endif // OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_
