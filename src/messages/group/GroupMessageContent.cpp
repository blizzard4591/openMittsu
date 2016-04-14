#include "messages/group/GroupMessageContent.h"

GroupMessageContent::GroupMessageContent(GroupId const& groupId) : groupId(groupId) {
	// Intentionally left empty.
}

GroupMessageContent::~GroupMessageContent() {
	// Intentionally left empty.
}

GroupId const& GroupMessageContent::getGroupId() const {
	return groupId;
}
