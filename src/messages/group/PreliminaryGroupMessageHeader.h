#ifndef OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_

#include "protocol/GroupId.h"
#include "messages/PreliminaryMessageHeader.h"

class PreliminaryGroupMessageHeader : public PreliminaryMessageHeader {
public:
	PreliminaryGroupMessageHeader(GroupId const& group, MessageId const& messageId, MessageFlags const& messageFlags);
	PreliminaryGroupMessageHeader(PreliminaryGroupMessageHeader const& other);
	virtual ~PreliminaryGroupMessageHeader();

	virtual GroupId const& getGroup() const;

	virtual PreliminaryGroupMessageHeader* clone() const;
private:
	GroupId const groupId;
};

#endif // OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_
