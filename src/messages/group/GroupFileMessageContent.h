#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"

#include <QString>

class GroupFileMessageContent : public GroupMessageContent {
public:
	GroupFileMessageContent(GroupId const& groupId);
	virtual ~GroupFileMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	friend class TypedMessageContentFactory<GroupFileMessageContent>;
private:
	QString const text;

	static bool registrationResult;

	GroupFileMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_
