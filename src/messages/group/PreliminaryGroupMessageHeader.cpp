#include "src/messages/group/PreliminaryGroupMessageHeader.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			PreliminaryGroupMessageHeader::PreliminaryGroupMessageHeader(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags) : PreliminaryMessageHeader(messageId, time, messageFlags), groupId(group) {
				// Intentionally left empty.
			}

			PreliminaryGroupMessageHeader::PreliminaryGroupMessageHeader(PreliminaryGroupMessageHeader const& other) : PreliminaryMessageHeader(other), groupId(other.groupId) {
				// Intentionally left empty.
			}

			PreliminaryGroupMessageHeader::~PreliminaryGroupMessageHeader() {
				// Intentionally left empty.
			}

			openmittsu::protocol::GroupId const& PreliminaryGroupMessageHeader::getGroup() const {
				return groupId;
			}

			PreliminaryGroupMessageHeader* PreliminaryGroupMessageHeader::clone() const {
				return new PreliminaryGroupMessageHeader(*this);
			}

		}
	}
}
