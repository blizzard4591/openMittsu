#include "MissingIdentityProcessor.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"

MissingIdentityProcessor::MissingIdentityProcessor(openmittsu::protocol::ContactId const& missingContact) : hasErrors(false), missingContacts(), queuedMessages(), groupIdPtr(nullptr) {
	missingContacts.insert(missingContact);
}

MissingIdentityProcessor::MissingIdentityProcessor(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& missingContacts) : hasErrors(false), missingContacts(missingContacts), queuedMessages(), groupIdPtr(std::make_unique<openmittsu::protocol::GroupId>(groupId)) {
	// Intentionally left empty.
}

MissingIdentityProcessor::~MissingIdentityProcessor() {
	// Intentionally left empty.
}

QSet<openmittsu::protocol::ContactId> const& MissingIdentityProcessor::getMissingContacts() const {
	return missingContacts;
}

bool MissingIdentityProcessor::hasFinishedSuccessfully() const {
	return (missingContacts.isEmpty() && !hasErrors);
}

bool MissingIdentityProcessor::hasFinished() const {
	return missingContacts.isEmpty();
}

std::list<openmittsu::messages::MessageWithEncryptedPayload> const& MissingIdentityProcessor::getQueuedMessages() const {
	return queuedMessages;
}

void MissingIdentityProcessor::enqueueMessage(openmittsu::messages::MessageWithEncryptedPayload const& message) {
	queuedMessages.push_back(message);
}

void MissingIdentityProcessor::identityFetcherTaskFinished(openmittsu::protocol::ContactId const& contactId, bool successful) {
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

openmittsu::protocol::GroupId const& MissingIdentityProcessor::getAssociatedGroupId() const {
	if (!hasAssociatedGroupId()) {
		throw openmittsu::exceptions::IllegalFunctionCallException() << "Can not get GroupId from MissingIdentityProcessor that does not have an associated GroupId.";
	} else {
		return *groupIdPtr;
	}
}
