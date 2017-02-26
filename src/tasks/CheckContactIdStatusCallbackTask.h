#ifndef OPENMITTSU_TASKS_CHECKCONTACTIDSTATUSCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CHECKCONTACTIDSTATUSCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "protocol/ContactIdStatus.h"
#include "ServerConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

class CheckContactIdStatusCallbackTask : public CertificateBasedCallbackTask {
public:
	CheckContactIdStatusCallbackTask(std::shared_ptr<ServerConfiguration> const& serverConfiguration, QList<ContactId> const& identitiesToCheck);
	virtual ~CheckContactIdStatusCallbackTask();

	QList<ContactId> const& getContactIds() const;
	QHash<ContactId, ContactIdStatus> const& getFetchedStatus() const;
protected:
	virtual void taskRun() override;
private:
	QString const urlString;
	QString const agentString;
	QList<ContactId> const identitiesToFetch;

	QHash<ContactId, ContactIdStatus> fetchedFeatureLevels;
};

#endif // OPENMITTSU_TASKS_CHECKCONTACTIDSTATUSCALLBACKTASK_H_