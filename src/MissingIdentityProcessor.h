#ifndef OPENMITTSU_GROUPMISSINGIDENTITYFETCHER_H_
#define OPENMITTSU_GROUPMISSINGIDENTITYFETCHER_H_

#include <QObject>
#include <QSet>
#include <QList>
#include <memory>
#include <utility>

#include "messages/MessageWithEncryptedPayload.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"

class MissingIdentityProcessor {
public:
	explicit MissingIdentityProcessor(ContactId const& missingContact);
	explicit MissingIdentityProcessor(GroupId const& groupId, QSet<ContactId> const& missingContacts);
	virtual ~MissingIdentityProcessor();

	QSet<ContactId> const& getMissingContacts() const;

	bool hasFinishedSuccessfully() const;
	bool hasFinished() const;

	QList<MessageWithEncryptedPayload> const& getQueuedMessages() const;
	void enqueueMessage(MessageWithEncryptedPayload const& message);

	void identityFetcherTaskFinished(ContactId const& contactId, bool successful);

	bool hasAssociatedGroupId() const;
	GroupId const& getAssociatedGroupId() const;
private:
	bool hasErrors;
	QSet<ContactId> missingContacts;
	QList<MessageWithEncryptedPayload> queuedMessages;
	std::unique_ptr<GroupId> const groupIdPtr;
};

#endif // OPENMITTSU_GROUPMISSINGIDENTITYFETCHER_H_
