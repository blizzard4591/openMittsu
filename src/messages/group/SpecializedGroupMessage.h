#ifndef OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_

#include "src/messages/Message.h"
#include "src/messages/FullMessageHeader.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/messages/group/PreliminaryGroupMessage.h"
#include "src/messages/group/UnspecializedGroupMessage.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace group {

			class SpecializedGroupMessage : public Message {
			public:
				SpecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent);
				SpecializedGroupMessage(UnspecializedGroupMessage const& message, openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& uniqueMessageId);
				SpecializedGroupMessage(SpecializedGroupMessage const& other);
				virtual ~SpecializedGroupMessage();

				virtual GroupMessageContent const* getGroupMessageContent() const;
				virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_SPECIALIZEDGROUPMESSAGE_H_
