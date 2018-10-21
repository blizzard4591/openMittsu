#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupFileMessageContent : public GroupMessageContent {
			public:
				GroupFileMessageContent(openmittsu::protocol::GroupId const& groupId);
				virtual ~GroupFileMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				friend class TypedMessageContentFactory<GroupFileMessageContent>;
			private:
				QString const text;

				static bool registrationResult;

				GroupFileMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPFILEMESSAGECONTENT_H_
