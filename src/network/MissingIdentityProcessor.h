#ifndef OPENMITTSU_NETWORK_MISSINGIDENTITYPROCESSOR_H_
#define OPENMITTSU_NETWORK_MISSINGIDENTITYPROCESSOR_H_

#include <QObject>
#include <QSet>
#include <list>
#include <memory>
#include <utility>

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

namespace openmittsu {
	namespace network {

		class MissingIdentityProcessor {
		public:
			explicit MissingIdentityProcessor(openmittsu::protocol::ContactId const& missingContact);
			explicit MissingIdentityProcessor(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& missingContacts);
			virtual ~MissingIdentityProcessor();

			QSet<openmittsu::protocol::ContactId> const& getMissingContacts() const;

			bool hasFinishedSuccessfully() const;
			bool hasFinished() const;

			std::list<openmittsu::messages::MessageWithEncryptedPayload> const& getQueuedMessages() const;
			void enqueueMessage(openmittsu::messages::MessageWithEncryptedPayload const& message);

			void identityFetcherTaskFinished(openmittsu::protocol::ContactId const& contactId, bool successful);

			bool hasAssociatedGroupId() const;
			openmittsu::protocol::GroupId const& getAssociatedGroupId() const;
		private:
			bool hasErrors;
			QSet<openmittsu::protocol::ContactId> missingContacts;
			std::list<openmittsu::messages::MessageWithEncryptedPayload> queuedMessages;
			std::unique_ptr<openmittsu::protocol::GroupId> const groupIdPtr;
		};

	}
}

#endif // OPENMITTSU_NETWORK_MISSINGIDENTITYPROCESSOR_H_
