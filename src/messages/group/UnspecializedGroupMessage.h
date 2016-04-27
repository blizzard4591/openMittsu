#ifndef OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_

#include "messages/Message.h"
#include "messages/FullMessageHeader.h"
#include "messages/group/GroupMessageContent.h"
#include "messages/group/PreliminaryGroupMessage.h"

class UnspecializedGroupMessage : public Message {
public:
	UnspecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent);
	UnspecializedGroupMessage(PreliminaryGroupMessage const& message, ContactId const& sender, PushFromId const& pushFromId);
	UnspecializedGroupMessage(UnspecializedGroupMessage const& other);
	virtual ~UnspecializedGroupMessage();

	virtual GroupMessageContent const* getGroupMessageContent() const;
	virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
};

#endif // OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_
