#ifndef OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_

#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"

class UniqueMessageIdGenerator {
public:
	virtual ~UniqueMessageIdGenerator() {
		//
	}

	virtual MessageId getNextUniqueMessageId(ContactId const& receiver) = 0;
	virtual MessageId getNextUniqueMessageId(GroupId const& receiver) = 0;
};

#endif // OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_
