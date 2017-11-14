#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_

#include "src/messages/MessageContent.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactMessageContent : public MessageContent {
			public:
				ContactMessageContent();
				virtual ~ContactMessageContent();

				virtual ContactMessageContent* clone() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTMESSAGECONTENT_H_
