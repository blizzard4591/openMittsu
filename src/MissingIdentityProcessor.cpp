#include "MissingIdentityProcessor.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "utility/Logging.h"

MissingIdentityProcessor::MissingIdentityProcessor(ContactId const& missingContact) : hasErrors(false), missingContacts(), queuedMessages(), groupIdPtr(nullptr) {
	missingContacts.insert(missingContact);
}

MissingIdentityProcessor::MissingIdentityProcessor(GroupId const& groupId, QSet<ContactId> const& missingContacts) : hasErrors(false), missingContacts(missingContacts), queuedMessages(), groupIdPtr(std::make_unique<GroupId>(groupId)) {
	// Intentionally left empty.
}

MissingIdentityProcessor::~MissingIdentityProcessor() {
	// Intentionally left empty.
}

QSet<ContactId> const& MissingIdentityProcessor::getMissingContacts() const {
	return missingContacts;
}

bool MissingIdentityProcessor::hasFinishedSuccessfully() const {
	return (missingContacts.isEmpty() && !hasErrors);
}

bool MissingIdentityProcessor::hasFinished() const {
	return missingContacts.isEmpty();
}

QList<MessageWithEncryptedPayload> const& MissingIdentityProcessor::getQueuedMessages() const {
	return queuedMessages;
}

void MissingIdentityProcessor::enqueueMessage(MessageWithEncryptedPayload const& message) {
	queuedMessages.append(message);
}

void MissingIdentityProcessor::identityFetcherTaskFinished(ContactId const& contactId, bool successful) {
	if (!missingContacts.contains(contactId)) {
		LOGGER()->warn("MissingIdentityProcessor received result for contact {}, but it was not set as missing in this context.", contactId.toString());
	} else {
		missingContacts.remove(contactId);
		if (!successful) {
			hasErrors = true;
		}
	}
}

bool MissingIdentityProcessor::hasAssociatedGroupId() const {
	return groupIdPtr.get() != nullptr;
}

GroupId const& MissingIdentityProcessor::getAssociatedGroupId() const {
	if (!hasAssociatedGroupId()) {
		throw IllegalFunctionCallException() << "Can not get GroupId from MissingIdentityProcessor that does not have an associated GroupId.";
	} else {
		return *groupIdPtr;
	}
}
