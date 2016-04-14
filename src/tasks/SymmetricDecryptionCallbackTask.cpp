#include "tasks/SymmetricDecryptionCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "utility/Logging.h"

#include <QString>

SymmetricDecryptionCallbackTask::SymmetricDecryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, Nonce const& nonce, ContactId const& symmetricKeyPartner) : MessageCallbackTask(message, acknowledgmentProcessor), cryptoBox(cryptoBox), encryptedData(encryptedData), nonce(nonce), symmetricKeyPartner(symmetricKeyPartner), decryptedData() {
	// Intentionally left empty.
}

SymmetricDecryptionCallbackTask::~SymmetricDecryptionCallbackTask() {
	// Intentionally left empty.
}

ContactId const& SymmetricDecryptionCallbackTask::getSymmetricKeyPartner() const {
	return symmetricKeyPartner;
}

QByteArray const& SymmetricDecryptionCallbackTask::getDecryptedData() const {
	return decryptedData;
}

QByteArray const& SymmetricDecryptionCallbackTask::getEncryptedData() const {
	return encryptedData;
}

Nonce const& SymmetricDecryptionCallbackTask::getNonce() const {
	return nonce;
}

void SymmetricDecryptionCallbackTask::taskRun() {
	LOGGER_DEBUG("Running SymmetricDecryptionCallbackTask for {} Bytes from user {}.", encryptedData.size(), symmetricKeyPartner.toString());
	try {
		decryptedData = cryptoBox->decrypt(encryptedData, nonce, symmetricKeyPartner);

		finishedWithNoError();
	} catch (CryptoException& cryptoException) {
		finishedWithError(-1, QString(cryptoException.what()));
	}
}
