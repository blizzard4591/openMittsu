#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPLOCATIONMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPLOCATIONMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"

#include "src/utility/Location.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupLocationMessageContent : public GroupMessageContent {
			public:
				GroupLocationMessageContent(openmittsu::protocol::GroupId const& groupId, double latitude, double longitude, double height, QString const& description);
				virtual ~GroupLocationMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				virtual double getLatitude() const;
				virtual double getLongitude() const;
				virtual double getHeight() const;
				virtual openmittsu::utility::Location getLocation() const;

				virtual QString const& getDescription() const;

				friend class TypedMessageContentFactory<GroupLocationMessageContent>;
			private:
				double const latitude;
				double const longitude;
				double const height;
				QString const description;

				static bool registrationResult;

				GroupLocationMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPLOCATIONMESSAGECONTENT_H_
