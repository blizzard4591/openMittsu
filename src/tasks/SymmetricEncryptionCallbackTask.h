#ifndef OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_

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

class SymmetricEncryptionCallbackTask : public MessageCallbackTask {
public:
	SymmetricEncryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, ContactId const& symmetricKeyPartner);
	virtual ~SymmetricEncryptionCallbackTask();

	ContactId const& getSymmetricKeyPartner() const;
	QByteArray const& getUnencryptedData() const;

	QByteArray const& getEncryptedData() const;
	Nonce const& getNonce() const;
protected:
	virtual void taskRun() override;
private:	
	CryptoBox* const cryptoBox;
	QByteArray const unencryptedData;
	ContactId const symmetricKeyPartner;
	QByteArray encryptedData;
	Nonce nonce;
};

#endif // OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_
