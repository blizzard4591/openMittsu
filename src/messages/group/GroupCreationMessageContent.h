#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/protocol/ContactId.h"

#include <QString>
#include <QSet>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupCreationMessageContent : public GroupMessageContent {
			public:
				GroupCreationMessageContent(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& groupMembers);
				virtual ~GroupCreationMessageContent();

				virtual QSet<openmittsu::protocol::ContactId> const& getGroupMembers() const;

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				friend class TypedMessageContentFactory<GroupCreationMessageContent>;
			private:
				QSet<openmittsu::protocol::ContactId> const members;

				static bool registrationResult;

				GroupCreationMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPCREATIONMESSAGECONTENT_H_
