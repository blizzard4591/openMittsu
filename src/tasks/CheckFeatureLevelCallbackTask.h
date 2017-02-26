#ifndef OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/FeatureLevel.h"
#include "protocol/Nonce.h"
#include "ServerConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

class CheckFeatureLevelCallbackTask : public CertificateBasedCallbackTask {
public:
	CheckFeatureLevelCallbackTask(std::shared_ptr<ServerConfiguration> const& serverConfiguration, QList<ContactId> const& identitiesToCheck);
	virtual ~CheckFeatureLevelCallbackTask();

	QList<ContactId> const& getContactIds() const;
	QHash<ContactId, FeatureLevel> const& getFetchedFeatureLevels() const;
protected:
	virtual void taskRun() override;
private:
	QString const urlString;
	QString const agentString;
	QList<ContactId> const identitiesToFetch;

	QHash<ContactId, FeatureLevel> fetchedFeatureLevels;
};

#endif // OPENMITTSU_TASKS_CHECKFEATURELEVELCALLBACKTASK_H_