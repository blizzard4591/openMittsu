#ifndef OPENMITTSU_TASKS_MESSAGEBASEDIDENTITYRECEIVERCALLBACKTASK_H_
#define OPENMITTSU_TASKS_MESSAGEBASEDIDENTITYRECEIVERCALLBACKTASK_H_

#include "messages/MessageWithEncryptedPayload.h"
#include "tasks/IdentityReceiverCallbackTask.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "ServerConfiguration.h"
#include "PublicKey.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

#include <utility>

class MessageBasedIdentityReceiverCallbackTask : public IdentityReceiverCallbackTask {
public:
	MessageBasedIdentityReceiverCallbackTask(ServerConfiguration* serverConfiguration, MessageWithEncryptedPayload const& message, ContactId const& identityToFetch);
	virtual ~MessageBasedIdentityReceiverCallbackTask();

	MessageWithEncryptedPayload const& getInitialMessage() const;
private:
	MessageWithEncryptedPayload const initialMessage;
};

#endif // OPENMITTSU_TASKS_MESSAGEBASEDIDENTITYRECEIVERCALLBACKTASK_H_