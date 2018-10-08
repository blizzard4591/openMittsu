#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYUSERMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYUSERMESSAGE_H_

#include "src/database/MediaFileItem.h"
#include "src/dataproviders/messages/ReadonlyMessage.h"
#include "src/dataproviders/messages/UserMessageState.h"
#include "src/utility/Location.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ReadonlyUserMessage : public virtual ReadonlyMessage {
			public:
				ReadonlyUserMessage() {}
				virtual ~ReadonlyUserMessage() {}

				virtual bool isRead() const = 0;

				/** Encodes whether all data is available, i.e. referenced image or video data is available. */
				virtual bool isSaved() const = 0;

				virtual UserMessageState const& getMessageState() const = 0;
				virtual openmittsu::protocol::MessageTime const& getReceivedAt() const = 0;
				virtual openmittsu::protocol::MessageTime const& getSeenAt() const = 0;
				virtual bool isStatusMessage() const = 0;

				virtual QString const& getContentAsText() const = 0;
				virtual openmittsu::utility::Location getContentAsLocation() const = 0;
				virtual openmittsu::database::MediaFileItem getContentAsImage() const = 0;

				virtual QString const& getCaption() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGES_READONLYUSERMESSAGE_H_
