#ifndef OPENMITTSU_TASKS_CHECKCONTACTACTIVITYSTATUSCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CHECKCONTACTACTIVITYSTATUSCALLBACKTASK_H_

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/AccountStatus.h"
#include "src/network/ServerConfiguration.h"

#include "src/tasks/CertificateBasedCallbackTask.h"
#include "src/tasks/CallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

namespace openmittsu {
	namespace tasks {

		class CheckContactActivityStatusCallbackTask : public CertificateBasedCallbackTask, public CallbackTask {
		public:
			CheckContactActivityStatusCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QSet<openmittsu::protocol::ContactId> const& identitiesToCheck);
			virtual ~CheckContactActivityStatusCallbackTask();

			QSet<openmittsu::protocol::ContactId> const& getContactIds() const;
			QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& getFetchedStatus() const;
		protected:
			virtual void taskRun() override;
		private:
			QString const m_urlString;
			QString const m_agentString;
			QSet<openmittsu::protocol::ContactId> const m_identitiesToFetch;

			QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> m_fetchedFeatureLevels;
		};

	}
}

#endif // OPENMITTSU_TASKS_CHECKCONTACTACTIVITYSTATUSCALLBACKTASK_H_
