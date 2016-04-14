#ifndef OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_

#include "messages/Message.h"
#include "messages/FullMessageHeader.h"
#include "messages/group/GroupMessageContent.h"
#include "messages/group/PreliminaryGroupMessage.h"
#include "messages/group/UnspecializedGroupMessage.h"

#include <memory>

class SpecializedGroupMessage : public Message {
public:
	SpecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent);
	SpecializedGroupMessage(UnspecializedGroupMessage const& message, ContactId const& receiver, MessageId const& uniqueMessageId);
	SpecializedGroupMessage(SpecializedGroupMessage const& other);
	virtual ~SpecializedGroupMessage();

	virtual GroupMessageContent const* getGroupMessageContent() const;
	virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
};

#endif // OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_
