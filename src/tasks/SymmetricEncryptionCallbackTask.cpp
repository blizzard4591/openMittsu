#include "tasks/SymmetricEncryptionCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "utility/Logging.h"

#include <QString>

SymmetricEncryptionCallbackTask::SymmetricEncryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, ContactId const& symmetricKeyPartner) : MessageCallbackTask(message, acknowledgmentProcessor), cryptoBox(cryptoBox), unencryptedData(data), symmetricKeyPartner(symmetricKeyPartner), encryptedData(), nonce() {
	// Intentionally left empty.
}

SymmetricEncryptionCallbackTask::~SymmetricEncryptionCallbackTask() {
	// Intentionally left empty.
}

ContactId const& SymmetricEncryptionCallbackTask::getSymmetricKeyPartner() const {
	return symmetricKeyPartner;
}

QByteArray const& SymmetricEncryptionCallbackTask::getUnencryptedData() const {
	return unencryptedData;
}

QByteArray const& SymmetricEncryptionCallbackTask::getEncryptedData() const {
	return encryptedData;
}

Nonce const& SymmetricEncryptionCallbackTask::getNonce() const {
	return nonce;
}

void SymmetricEncryptionCallbackTask::taskRun() {
	LOGGER_DEBUG("Running SymmetricEncryptionCallbackTask for {} Bytes for user {}.", unencryptedData.size(), symmetricKeyPartner.toString());
	try {
		std::pair<Nonce, QByteArray> result = cryptoBox->encrypt(unencryptedData, symmetricKeyPartner);

		this->nonce = result.first;
		this->encryptedData = result.second;

		finishedWithNoError();
	} catch (CryptoException& cryptoException) {
		finishedWithError(-1, QString(cryptoException.what()));
	}
}
