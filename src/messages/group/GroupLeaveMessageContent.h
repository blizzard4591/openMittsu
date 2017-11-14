#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupLeaveMessageContent : public GroupMessageContent {
			public:
				GroupLeaveMessageContent(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::ContactId const& leavingContactId);
				virtual ~GroupLeaveMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				virtual openmittsu::protocol::ContactId const& getLeavingContactId() const;

				friend class TypedMessageContentFactory<GroupLeaveMessageContent>;
			private:
				openmittsu::protocol::ContactId const leavingContactId;

				static bool registrationResult;

				GroupLeaveMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPLEAVEMESSAGECONTENT_H_
