#ifndef OPENMITTSU_TASKS_KEYANDFIXEDNONCEDECRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_KEYANDFIXEDNONCEDECRYPTIONCALLBACKTASK_H_

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

class KeyAndFixedNonceDecryptionCallbackTask : public MessageCallbackTask {
public:
	KeyAndFixedNonceDecryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, EncryptionKey const& encryptionKey, Nonce const& nonce);
	virtual ~KeyAndFixedNonceDecryptionCallbackTask();

	EncryptionKey const& getEncryptionKey() const;
	QByteArray const& getDecryptedData() const;

	QByteArray const& getEncryptedData() const;
	Nonce const& getFixedNonce() const;
protected:
	virtual void taskRun() override;
private:	
	CryptoBox* const cryptoBox;
	QByteArray const encryptedData;
	Nonce const nonce;
	EncryptionKey const encryptionKey;
	QByteArray decryptedData;
};

#endif // OPENMITTSU_TASKS_KEYANDFIXEDNONCEDECRYPTIONCALLBACKTASK_H_
