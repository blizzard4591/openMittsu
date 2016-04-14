#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"

#include <QString>

class GroupTextMessageContent : public GroupMessageContent {
public:
	GroupTextMessageContent(GroupId const& groupId, QString const& text);
	virtual ~GroupTextMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	virtual QString const& getText() const;

	friend class TypedMessageContentFactory<GroupTextMessageContent>;
private:
	QString const text;

	static bool registrationResult;

	GroupTextMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
