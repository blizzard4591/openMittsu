#ifndef OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGE_H_
#define OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGE_H_

#include "src/messages/contact/PreliminaryContactMessageHeader.h"
#include "src/messages/contact/ContactMessageContent.h"

#include <QMetaType>

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class PreliminaryContactMessage {
			public:
				PreliminaryContactMessage(PreliminaryContactMessageHeader* preliminaryMessageHeader, ContactMessageContent* messageContent);
				PreliminaryContactMessage(PreliminaryContactMessage const& other);
				virtual ~PreliminaryContactMessage();

				PreliminaryContactMessageHeader const* getPreliminaryMessageHeader() const;
				ContactMessageContent const* getPreliminaryMessageContent() const;

				friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<PreliminaryContactMessage, true>;
			private:
				std::unique_ptr<PreliminaryContactMessageHeader const> const preliminaryMessageHeader;
				std::unique_ptr<ContactMessageContent const> const messageContent;

				// Disable the default constructor
				PreliminaryContactMessage();
			};

		}
	}
}

Q_DECLARE_METATYPE(openmittsu::messages::contact::PreliminaryContactMessage)

#endif // OPENMITTSU_MESSAGES_CONTACT_PRELIMINARYCONTACTMESSAGE_H_
