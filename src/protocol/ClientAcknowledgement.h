#ifndef OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_
#define OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"

class ClientAcknowledgement {
public:
	explicit ClientAcknowledgement(ContactId const& sender, MessageId const& messageId);
	virtual ~ClientAcknowledgement();

	virtual QByteArray toPacket() const;
private:
	ContactId const sender;
	MessageId const messageId;
};

#endif // OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_
