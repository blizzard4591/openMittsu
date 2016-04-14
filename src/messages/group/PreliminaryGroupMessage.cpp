#include "messages/group/PreliminaryGroupMessage.h"

PreliminaryGroupMessage::PreliminaryGroupMessage() : preliminaryMessageHeader(nullptr), messageContent(nullptr) {
	// This constructor is disabled and should not be used.
	throw;
}

PreliminaryGroupMessage::PreliminaryGroupMessage(PreliminaryGroupMessageHeader* preliminaryMessageHeader, GroupMessageContent* messageContent) : preliminaryMessageHeader(preliminaryMessageHeader), messageContent(messageContent) {
	// Intentionally left empty.
}

PreliminaryGroupMessage::PreliminaryGroupMessage(PreliminaryGroupMessage const& other) : preliminaryMessageHeader(other.preliminaryMessageHeader->clone()), messageContent(other.messageContent->clone()) {
	// Intentionally left empty.
}

PreliminaryGroupMessage::~PreliminaryGroupMessage() {
	// Intentionally left empty.
}

PreliminaryGroupMessageHeader const* PreliminaryGroupMessage::getPreliminaryMessageHeader() const {
	return preliminaryMessageHeader.get();
}

GroupMessageContent const* PreliminaryGroupMessage::getPreliminaryMessageContent() const {
	return messageContent.get();
}

