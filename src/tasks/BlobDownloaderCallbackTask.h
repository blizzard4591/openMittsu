#ifndef OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_

#include "messages/Message.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "tasks/MessageCallbackTask.h"
#include "ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

class BlobDownloaderCallbackTask : public virtual CertificateBasedCallbackTask, public virtual MessageCallbackTask {
public:
	BlobDownloaderCallbackTask(ServerConfiguration* serverConfiguration, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId);
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

#endif // OPENMITTSU_TASKS_BLOBDOWNLOADERCALLBACKTASK_H_