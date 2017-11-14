#ifndef OPENMITTSU_DATAPROVIDERS_GROUPMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_GROUPMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/dataproviders/messages/UserMessage.h"
#include "src/dataproviders/messages/GroupMessageType.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class GroupMessage : public virtual UserMessage {
			public:
				GroupMessage() {}
				virtual ~GroupMessage() {}

				virtual openmittsu::protocol::GroupId const& getGroupId() const = 0;
				virtual GroupMessageType getMessageType() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_GROUPMESSAGE_H_
