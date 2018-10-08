#ifndef OPENMITTSU_DATAPROVIDERS_READONLYGROUPMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_READONLYGROUPMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/dataproviders/messages/ReadonlyUserMessage.h"
#include "src/dataproviders/messages/GroupMessageType.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ReadonlyGroupMessage : public virtual ReadonlyUserMessage {
			public:
				ReadonlyGroupMessage() {}
				virtual ~ReadonlyGroupMessage() {}

				virtual openmittsu::protocol::GroupId const& getGroupId() const = 0;
				virtual GroupMessageType const& getMessageType() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_READONLYGROUPMESSAGE_H_
