#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPSYNCMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPSYNCMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupSyncMessageContent : public GroupMessageContent {
			public:
				GroupSyncMessageContent(openmittsu::protocol::GroupId const& groupId);
				virtual ~GroupSyncMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				friend class TypedMessageContentFactory<GroupSyncMessageContent>;
			private:
				static bool registrationResult;

				GroupSyncMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
