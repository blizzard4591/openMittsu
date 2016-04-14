#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_

#include "messages/MessageContent.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"

#include <QByteArray>

class GroupMessageContent : public MessageContent {
public:
	GroupMessageContent(GroupId const& groupId);

	virtual ~GroupMessageContent();

	virtual GroupMessageContent* clone() const = 0;

	virtual GroupId const& getGroupId() const;
private:
	GroupId const groupId;
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_
