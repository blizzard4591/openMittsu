#ifndef OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/tasks/CertificateBasedCallbackTask.h"
#include "src/tasks/MessageCallbackTask.h"
#include "src/network/ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

namespace openmittsu {
	namespace tasks {

		class BlobDeleterCallbackTask : public CertificateBasedCallbackTask, public MessageCallbackTask {
		public:
			BlobDeleterCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId);
			virtual ~BlobDeleterCallbackTask();
		protected:
			virtual void taskRun() override;
		private:
			QString const urlFinishedString;
			QString const agentString;

			QByteArray const blobId;
		};

	}
}

#endif // OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_