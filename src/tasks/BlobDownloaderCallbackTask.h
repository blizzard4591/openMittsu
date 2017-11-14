#ifndef OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/tasks/CertificateBasedCallbackTask.h"
#include "src/tasks/MessageCallbackTask.h"
#include "src/network/ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

namespace openmittsu {
	namespace tasks {

		class BlobDownloaderCallbackTask : public CertificateBasedCallbackTask, public MessageCallbackTask {
		public:
			BlobDownloaderCallbackTask(std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId);
			virtual ~BlobDownloaderCallbackTask();

			QByteArray const& getDownloadedBlob() const;
		protected:
			virtual void taskRun() override;
		private:
			QString const urlString;
			QString const agentString;

			QByteArray const blobId;
			QByteArray result;
		};

	}
}

#endif // OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_