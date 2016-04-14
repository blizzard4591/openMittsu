#ifndef OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_
#define OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include <QtGlobal>
#include <QByteArray>
#include <QMetaType>

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

Q_DECLARE_METATYPE(ContactIdWithMessageId)

#endif // #define OPENMITTSU_PROTOCOL_CONTACTIDWITHMESSAGEID_H_

