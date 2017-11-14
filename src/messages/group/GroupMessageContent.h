#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_

#include "src/messages/MessageContent.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupMessageContent : public MessageContent {
			public:
				GroupMessageContent(openmittsu::protocol::GroupId const& groupId);
				virtual ~GroupMessageContent();

				virtual GroupMessageContent* clone() const = 0;

				virtual openmittsu::protocol::GroupId const& getGroupId() const;
			private:
				openmittsu::protocol::GroupId const groupId;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPMESSAGECONTENT_H_
