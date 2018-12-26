#ifndef OPENMITTSU_TASKS_SAFECALLBACKTASK_H_
#define OPENMITTSU_TASKS_SAFECALLBACKTASK_H_

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/network/ServerConfiguration.h"

#include "src/crypto/SafeMasterKey.h"
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

		class SafeCallbackTask : public CertificateBasedCallbackTask, public CallbackTask {
		public:
			SafeCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword, QString const& serviceUrl);
			virtual ~SafeCallbackTask();
		protected:
			openmittsu::crypto::SafeMasterKey const m_safeMasterKey;
			QString const m_urlString;
			QString const m_agentString;
			openmittsu::protocol::ContactId const m_identity;
			QString const m_safePassword;

			static QString makeUrl(QString const& userDefinedSafeServer, QString const& defaultSafeServer, openmittsu::crypto::SafeMasterKey const& safeMasterKey, QString const& serviceSubUrl);
		};

	}
}

#endif // OPENMITTSU_TASKS_SAFECALLBACKTASK_H_
