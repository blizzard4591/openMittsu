#ifndef OPENMITTSU_PROTOCOL_SIMPLEUNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_PROTOCOL_SIMPLEUNIQUEMESSAGEIDGENERATOR_H_

#include "protocol/UniqueMessageIdGenerator.h"

class SimpleUniqueMessageIdGenerator : public UniqueMessageIdGenerator {
public:
	virtual ~SimpleUniqueMessageIdGenerator();

	virtual MessageId getNextUniqueMessageId(ContactId const& receiver) override;
	virtual MessageId getNextUniqueMessageId(GroupId const& receiver) override;
};

#endif // OPENMITTSU_PROTOCOL_SIMPLEUNIQUEMESSAGEIDGENERATOR_H_
