#ifndef OPENMITTSU_DATAPROVIDERS_USERMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_USERMESSAGE_H_

#include "src/database/MediaFileItem.h"
#include "src/dataproviders/messages/Message.h"
#include "src/dataproviders/messages/UserMessageState.h"
#include "src/utility/Location.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class UserMessage : public virtual Message {
			public:
				UserMessage() {}
				virtual ~UserMessage() {}

				virtual bool isRead() const = 0;

				/** Encodes whether all data is available, i.e. referenced image or video data is available. */
				virtual bool isSaved() const = 0;

				virtual UserMessageState getMessageState() const = 0;
				virtual void setMessageState(UserMessageState const& messageState, openmittsu::protocol::MessageTime const& when) = 0;

				virtual openmittsu::protocol::MessageTime getReceivedAt() const = 0;
				virtual openmittsu::protocol::MessageTime getSeenAt() const = 0;
				virtual bool isStatusMessage() const = 0;

				virtual QString getContentAsText() const = 0;
				virtual openmittsu::utility::Location getContentAsLocation() const = 0;
				virtual openmittsu::database::MediaFileItem getContentAsMediaFile() const = 0;

				virtual QString getCaption() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_USERMESSAGE_H_
