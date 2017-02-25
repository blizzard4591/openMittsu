#ifndef OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "ServerConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>


class CheckFeatureLevelCallbackTask : public CertificateBasedCallbackTask {
public:
	CheckFeatureLevelCallbackTask(ServerConfiguration* serverConfiguration, QList<ContactId> const& identitiesToCheck);
	virtual ~CheckFeatureLevelCallbackTask();

	QList<ContactId> const& getContactIds() const;
	QHash<ContactId, int> const& getFetchedFeatureLevels() const;
protected:
	virtual void taskRun() override;
private:
	QString const urlString;
	QString const agentString;
	QList<ContactId> const identitiesToFetch;

	QHash<ContactId, int> fetchedFeatureLevels;
};

#endif // OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_