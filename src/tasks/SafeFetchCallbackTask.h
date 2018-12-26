#ifndef OPENMITTSU_TASKS_SAFEFETCHCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SAFEFETCHCALLBACKTASK_H_

#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ContactId.h"
#include "src/network/ServerConfiguration.h"

#include "src/tasks/SafeCallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

namespace openmittsu {
	namespace tasks {

		class SafeFetchCallbackTask : public SafeCallbackTask {
		public:
			SafeFetchCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, QString const& userDefinedSafeServer, openmittsu::protocol::ContactId const& identity, QString const& safePassword);
			virtual ~SafeFetchCallbackTask();
		protected:
			virtual void taskRun() override;
		private:
			//
		};

	}
}

#endif // OPENMITTSU_TASKS_SAFEFETCHCALLBACKTASK_H_
