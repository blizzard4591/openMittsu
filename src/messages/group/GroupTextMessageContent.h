#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupTextMessageContent : public GroupMessageContent {
			public:
				GroupTextMessageContent(openmittsu::protocol::GroupId const& groupId, QString const& text);
				virtual ~GroupTextMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				virtual QString const& getText() const;

				friend class TypedMessageContentFactory<GroupTextMessageContent>;
			private:
				QString const text;

				static bool registrationResult;

				GroupTextMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPTEXTMESSAGECONTENT_H_
