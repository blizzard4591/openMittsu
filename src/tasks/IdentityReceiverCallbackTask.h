#ifndef OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/CertificateBasedCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "ServerConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

#include <utility>

class IdentityReceiverCallbackTask : public CertificateBasedCallbackTask {
public:
	IdentityReceiverCallbackTask(ServerConfiguration* serverConfiguration, ContactId const& identityToFetch);
	virtual ~IdentityReceiverCallbackTask();

	ContactId const& getContactIdOfFetchedPublicKey() const;
	PublicKey const& getFetchedPublicKey() const;
protected:
	virtual void taskRun() override;
private:
	QString const urlString;
	QString const agentString;
	ContactId const identityToFetch;

	PublicKey fetchedPublicKey;
};

#endif // OPENMITTSU_TASKS_IDENTITYRECEIVERCALLBACKTASK_H_