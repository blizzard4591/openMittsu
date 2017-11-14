#ifndef OPENMITTSU_DATAPROVIDERS_GROUPMESSAGECURSOR_H_
#define OPENMITTSU_DATAPROVIDERS_GROUPMESSAGECURSOR_H_

#include <memory>

#include "src/dataproviders/messages/GroupMessage.h"
#include "src/dataproviders/messages/MessageCursor.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class GroupMessageCursor : public virtual MessageCursor {
			public:
				GroupMessageCursor() {}
				virtual ~GroupMessageCursor() {}

				virtual openmittsu::protocol::GroupId const& getGroupId() const = 0;
				virtual std::shared_ptr<GroupMessage> getMessage() const = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_GROUPMESSAGECURSOR_H_
