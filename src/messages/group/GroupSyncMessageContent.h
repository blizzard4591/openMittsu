#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPSYNCMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPSYNCMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"

#include <QString>

class GroupSyncMessageContent : public GroupMessageContent {
public:
	GroupSyncMessageContent(GroupId const& groupId);
	virtual ~GroupSyncMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	friend class TypedMessageContentFactory<GroupSyncMessageContent>;
private:
	static bool registrationResult;

	GroupSyncMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
