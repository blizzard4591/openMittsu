#ifndef OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/tasks/CertificateBasedCallbackTask.h"
#include "src/tasks/MessageCallbackTask.h"
#include "src/network/ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

namespace openmittsu {
	namespace tasks {

		class BlobUploaderCallbackTask : public CertificateBasedCallbackTask, public MessageCallbackTask {
		public:
			BlobUploaderCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& dataToUpload);
			virtual ~BlobUploaderCallbackTask();

			QByteArray const& getBlobId() const;
		protected:
			virtual void taskRun() override;
		private:
			QString const urlString;
			QString const agentString;

			QByteArray const dataToUpload;
			QByteArray result;
		};

	}
}

#endif // OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_
