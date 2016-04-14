#ifndef OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_

#include "messages/Message.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "tasks/MessageCallbackTask.h"
#include "ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

class BlobUploaderCallbackTask : public virtual CertificateBasedCallbackTask, public virtual MessageCallbackTask {
public:
	BlobUploaderCallbackTask(ServerConfiguration* serverConfiguration, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& dataToUpload);
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

#endif // OPENMITTSU_TASKS_BLOBUPLOADERCALLBACKTASK_H_