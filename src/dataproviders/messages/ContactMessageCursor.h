#ifndef OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGECURSOR_H_
#define OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGECURSOR_H_

#include <memory>

#include "src/dataproviders/messages/ContactMessage.h"
#include "src/dataproviders/messages/MessageCursor.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ContactMessageCursor : public virtual MessageCursor {
			public:
				ContactMessageCursor() {}
				virtual ~ContactMessageCursor() {}

				virtual openmittsu::protocol::ContactId const& getContactId() const = 0;
				virtual std::shared_ptr<ContactMessage> getMessage() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGECURSOR_H_
