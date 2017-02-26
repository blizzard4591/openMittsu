#ifndef OPENMITTSU_TASKS_SETFEATURELEVELCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SETFEATURELEVELCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/FeatureLevel.h"
#include "protocol/Nonce.h"
#include "ServerConfiguration.h"
#include "ClientConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSslCertificate>
#include <memory>

class SetFeatureLevelCallbackTask : public CertificateBasedCallbackTask {
public:
	SetFeatureLevelCallbackTask(std::shared_ptr<ServerConfiguration> const& serverConfiguration, std::shared_ptr<CryptoBox> const& cryptoBox, std::shared_ptr<ClientConfiguration> const& clientConfiguration, FeatureLevel const& featureLevel);
	virtual ~SetFeatureLevelCallbackTask();

	FeatureLevel const& getFeatureLevelToSet() const;
protected:
	virtual void taskRun() override;
private:
	QString const urlString;
	QString const agentString;
	std::shared_ptr<CryptoBox> const cryptoBox;

	ContactId const contactId;
	FeatureLevel const featureLevelToSet;
};

#endif // OPENMITTSU_TASKS_SETFEATURELEVELCALLBACKTASK_H_