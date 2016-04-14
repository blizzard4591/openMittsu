#ifndef OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_

#include "messages/Message.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "tasks/MessageCallbackTask.h"
#include "ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

class BlobDeleterCallbackTask : public virtual CertificateBasedCallbackTask, public virtual MessageCallbackTask {
public:
	BlobDeleterCallbackTask(ServerConfiguration* serverConfiguration, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& blobId);
	virtual ~BlobDeleterCallbackTask();
protected:
	virtual void taskRun() override;
private:
	QString const urlFinishedString;
	QString const agentString;
	
	QByteArray const blobId;
};

#endif // OPENMITTSU_TASKS_BLOBDELETERCALLBACKTASK_H_