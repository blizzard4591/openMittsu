#ifndef OPENMITTSU_TASKS_FETCHPRIVATEINFORMATIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_FETCHPRIVATEINFORMATIONCALLBACKTASK_H_

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/FeatureLevel.h"
#include "src/network/ServerConfiguration.h"
#include "src/crypto/PublicKey.h"

#include "src/tasks/CallbackTask.h"
#include "src/tasks/CertificateBasedCallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

namespace openmittsu {
	namespace tasks {

		class FetchPrivateInformationCallbackTask : public CertificateBasedCallbackTask, public CallbackTask {
		public:
			FetchPrivateInformationCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::crypto::BasicCryptoBox const& cryptoBox, openmittsu::protocol::ContactId const& ownId);
			virtual ~FetchPrivateInformationCallbackTask();

			QString const& getServerGroup() const;
		protected:
			virtual void taskRun() override;
		private:
			QString const m_urlString;
			QString const m_agentString;
			openmittsu::crypto::BasicCryptoBox m_cryptoBox;

			openmittsu::protocol::ContactId const m_contactId;

			QString m_serverGroup;
		};

	}
}

#endif // OPENMITTSU_TASKS_FETCHPRIVATEINFORMATIONCALLBACKTASK_H_
