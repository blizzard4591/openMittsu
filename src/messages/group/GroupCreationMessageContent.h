#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"
#include "protocol/ContactId.h"

#include <QString>
#include <QSet>

class GroupCreationMessageContent : public GroupMessageContent {
public:
	GroupCreationMessageContent(GroupId const& groupId, QSet<ContactId> const groupMembers);
	virtual ~GroupCreationMessageContent();

	virtual QSet<ContactId> const& getGroupMembers() const;

	virtual GroupMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	friend class TypedMessageContentFactory<GroupCreationMessageContent>;
private:
	QSet<ContactId> const members;

	static bool registrationResult;

	GroupCreationMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_
