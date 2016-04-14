#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"

#include <QString>

class GroupLeaveMessageContent : public GroupMessageContent {
public:
	GroupLeaveMessageContent(GroupId const& groupId, ContactId const& leavingContactId);
	virtual ~GroupLeaveMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	virtual ContactId const& getLeavingContactId() const;

	friend class TypedMessageContentFactory<GroupLeaveMessageContent>;
private:
	ContactId const leavingContactId;

	static bool registrationResult;

	GroupLeaveMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_
