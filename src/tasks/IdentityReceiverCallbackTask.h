#ifndef OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/network/ServerConfiguration.h"
#include "src/crypto/PublicKey.h"

#include "src/tasks/CallbackTask.h"
#include "src/tasks/CertificateBasedCallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

#include <utility>

namespace openmittsu {
	namespace tasks {

		class IdentityReceiverCallbackTask : public CertificateBasedCallbackTask, public CallbackTask {
		public:
			IdentityReceiverCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::protocol::ContactId const& identityToFetch);
			virtual ~IdentityReceiverCallbackTask();

			openmittsu::protocol::ContactId const& getContactIdOfFetchedPublicKey() const;
			openmittsu::crypto::PublicKey const& getFetchedPublicKey() const;
		protected:
			virtual void taskRun() override;
		private:
			QString const m_urlString;
			QString const m_agentString;
			openmittsu::protocol::ContactId const m_identityToFetch;

			openmittsu::crypto::PublicKey m_fetchedPublicKey;
		};

	}
}

#endif // OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_