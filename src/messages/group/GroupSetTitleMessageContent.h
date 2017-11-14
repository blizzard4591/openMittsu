#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPSETTITLEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPSETTITLEMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupSetTitleMessageContent : public GroupMessageContent {
			public:
				GroupSetTitleMessageContent(openmittsu::protocol::GroupId const& groupId, QString const& title);
				virtual ~GroupSetTitleMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				virtual QString const& getTitle() const;

				friend class TypedMessageContentFactory<GroupSetTitleMessageContent>;
			private:
				QString const title;

				static bool registrationResult;

				GroupSetTitleMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPSETTITLEMESSAGECONTENT_H_
