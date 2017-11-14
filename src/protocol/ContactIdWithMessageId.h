#ifndef OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_
#define OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>

namespace openmittsu {
	namespace protocol {

		class ContactIdWithMessageId {
		public:
			ContactIdWithMessageId(ContactId const& contact, MessageId const& messageId);
			ContactIdWithMessageId(ContactIdWithMessageId const& other);
			virtual ~ContactIdWithMessageId();

			MessageId const& getMessageId() const;
			ContactId const& getContactId() const;

			bool operator ==(ContactIdWithMessageId const& other) const;

			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ContactIdWithMessageId, true>;
		private:
			ContactId const contact;
			MessageId const messageId;

			ContactIdWithMessageId();
		};

		uint qHash(ContactIdWithMessageId const& key, uint seed);
	}
}

Q_DECLARE_METATYPE(openmittsu::protocol::ContactIdWithMessageId)

#endif // #define OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_

