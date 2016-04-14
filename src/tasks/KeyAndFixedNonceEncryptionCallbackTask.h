#ifndef OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_

#include "messages/Message.h"
#include "protocol/ContactId.h"
#include "protocol/CryptoBox.h"
#include "protocol/EncryptionKey.h"
#include "protocol/Nonce.h"
#include "tasks/MessageCallbackTask.h"
#include "ServerConfiguration.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

class KeyAndFixedNonceEncryptionCallbackTask : public MessageCallbackTask {
public:
	KeyAndFixedNonceEncryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, Nonce const& nonce);
	virtual ~KeyAndFixedNonceEncryptionCallbackTask();

	EncryptionKey const& getEncryptionKey() const;
	QByteArray const& getUnencryptedData() const;

	QByteArray const& getEncryptedData() const;
	Nonce const& getFixedNonce() const;
protected:
	virtual void taskRun() override;
private:	
	CryptoBox* const cryptoBox;
	QByteArray const unencryptedData;
	Nonce const nonce;
	EncryptionKey const encryptionKey;
	QByteArray encryptedData;
};

#endif // OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_
