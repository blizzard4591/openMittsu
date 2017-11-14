#ifndef OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_
#define OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_

#include "src/messages/Message.h"
#include "src/messages/FullMessageHeader.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/messages/group/PreliminaryGroupMessage.h"

#include <QSet>

namespace openmittsu {
	namespace messages {
		namespace group {

			class UnspecializedGroupMessage : public openmittsu::messages::Message {
			public:
				UnspecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent, QSet<openmittsu::protocol::ContactId> const& recipients);
				UnspecializedGroupMessage(PreliminaryGroupMessage const& message, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId);
				UnspecializedGroupMessage(UnspecializedGroupMessage const& other);
				virtual ~UnspecializedGroupMessage();

				virtual GroupMessageContent const* getGroupMessageContent() const;
				virtual Message* withNewMessageContent(MessageContent* messageContent) const override;
				QSet<openmittsu::protocol::ContactId> const& getRecipients() const;
			private:
				QSet<openmittsu::protocol::ContactId> const m_recipients;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_UNSPECIALIZEDGROUPMESSAGE_H_
