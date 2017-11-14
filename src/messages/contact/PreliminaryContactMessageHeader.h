#ifndef OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/messages/PreliminaryMessageHeader.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			class PreliminaryContactMessageHeader : public PreliminaryMessageHeader {
			public:
				PreliminaryContactMessageHeader(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags);
				PreliminaryContactMessageHeader(PreliminaryContactMessageHeader const& other);
				virtual ~PreliminaryContactMessageHeader();

				virtual openmittsu::protocol::ContactId const& getReceiver() const;

				virtual PreliminaryContactMessageHeader* clone() const;
			private:
				openmittsu::protocol::ContactId const receiverId;

				// Disable the default constructor
				PreliminaryContactMessageHeader();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGEHEADER_H_
