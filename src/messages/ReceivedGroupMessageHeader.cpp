#include "src/messages/ReceivedGroupMessageHeader.h"

namespace openmittsu {
	namespace messages {

		ReceivedGroupMessageHeader::ReceivedGroupMessageHeader() : ReceivedMessageHeader(), group(openmittsu::protocol::GroupId(0, 0)) {
			//
		}

		ReceivedGroupMessageHeader::ReceivedGroupMessageHeader(FullMessageHeader const& header, openmittsu::protocol::GroupId const& group) : ReceivedMessageHeader(header), group(group) {
			// Intentionally left empty.
		}

		ReceivedGroupMessageHeader::~ReceivedGroupMessageHeader() {
			// Intentionally left empty.
		}

		openmittsu::protocol::GroupId const& ReceivedGroupMessageHeader::getGroupId() const {
			return group;
		}

	}
}
