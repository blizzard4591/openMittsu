#include "messages/group/PreliminaryGroupMessageHeader.h"

PreliminaryGroupMessageHeader::PreliminaryGroupMessageHeader(GroupId const& group, MessageId const& messageId, MessageFlags const& messageFlags) : PreliminaryMessageHeader(messageId, messageFlags), groupId(group) {
	// Intentionally left empty.
}

PreliminaryGroupMessageHeader::PreliminaryGroupMessageHeader(PreliminaryGroupMessageHeader const& other) : PreliminaryMessageHeader(other), groupId(other.groupId) {
	// Intentionally left empty.
}

PreliminaryGroupMessageHeader::~PreliminaryGroupMessageHeader() {
	// Intentionally left empty.
}

GroupId const& PreliminaryGroupMessageHeader::getGroup() const {
	return groupId;
}

PreliminaryGroupMessageHeader* PreliminaryGroupMessageHeader::clone() const {
	return new PreliminaryGroupMessageHeader(*this);
}
