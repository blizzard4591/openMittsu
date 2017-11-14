#ifndef OPENMITTSU_MESSAGES_CONTACT_CONTACTLOCATIONMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_CONTACTLOCATIONMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/contact/ContactMessageContent.h"

#include "src/utility/Location.h"

#include <QString>

namespace openmittsu {
	namespace messages {
		namespace contact {

			class ContactLocationMessageContent : public ContactMessageContent {
			public:
				ContactLocationMessageContent(double latitude, double longitude, double height, QString const& description);
				virtual ~ContactLocationMessageContent();

				virtual ContactMessageContent* clone() const override;

				virtual QByteArray toPacketPayload() const override;

				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				virtual double getLatitude() const;
				virtual double getLongitude() const;
				virtual double getHeight() const;
				virtual openmittsu::utility::Location getLocation() const;

				virtual QString const& getDescription() const;

				friend class TypedMessageContentFactory<ContactLocationMessageContent>;
			private:
				double const latitude;
				double const longitude;
				double const height;
				QString const description;

				static bool registrationResult;

				ContactLocationMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_CONTACT_CONTACTLOCATIONMESSAGECONTENT_H_
