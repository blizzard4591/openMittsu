#include "src/messages/group/GroupMessageContent.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupMessageContent::GroupMessageContent(openmittsu::protocol::GroupId const& groupId) : groupId(groupId) {
				// Intentionally left empty.
			}

			GroupMessageContent::~GroupMessageContent() {
				// Intentionally left empty.
			}

			openmittsu::protocol::GroupId const& GroupMessageContent::getGroupId() const {
				return groupId;
			}

		}
	}
}
