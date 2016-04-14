#ifndef OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_

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

class SymmetricDecryptionCallbackTask : public MessageCallbackTask {
public:
	SymmetricDecryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, Nonce const& nonce, ContactId const& symmetricKeyPartner);
	virtual ~SymmetricDecryptionCallbackTask();

	ContactId const& getSymmetricKeyPartner() const;
	QByteArray const& getDecryptedData() const;

	QByteArray const& getEncryptedData() const;
	Nonce const& getNonce() const;
protected:
	virtual void taskRun() override;
private:	
	CryptoBox* const cryptoBox;
	QByteArray const encryptedData;
	Nonce const nonce;
	ContactId const symmetricKeyPartner;
	QByteArray decryptedData;
};

#endif // OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_
