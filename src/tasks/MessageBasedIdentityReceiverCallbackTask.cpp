#include "tasks/MessageBasedIdentityReceiverCallbackTask.h"

MessageBasedIdentityReceiverCallbackTask::MessageBasedIdentityReceiverCallbackTask(ServerConfiguration* serverConfiguration, MessageWithEncryptedPayload const& message, ContactId const& identityToFetch) : IdentityReceiverCallbackTask(serverConfiguration, identityToFetch), initialMessage(message) {
	// Intentionally left empty.
}

MessageBasedIdentityReceiverCallbackTask::~MessageBasedIdentityReceiverCallbackTask() {
	// Intentionally left empty.
}

MessageWithEncryptedPayload const& MessageBasedIdentityReceiverCallbackTask::getInitialMessage() const {
	return initialMessage;
}
