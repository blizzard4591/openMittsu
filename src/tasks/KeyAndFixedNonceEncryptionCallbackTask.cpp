#include "tasks/KeyAndFixedNonceEncryptionCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "utility/Logging.h"

#include <QString>

KeyAndFixedNonceEncryptionCallbackTask::KeyAndFixedNonceEncryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, Nonce const& nonce) : MessageCallbackTask(message, acknowledgmentProcessor), cryptoBox(cryptoBox), unencryptedData(data), nonce(nonce), encryptionKey(), encryptedData() {
	// Intentionally left empty.
}

KeyAndFixedNonceEncryptionCallbackTask::~KeyAndFixedNonceEncryptionCallbackTask() {
	// Intentionally left empty.
}

EncryptionKey const& KeyAndFixedNonceEncryptionCallbackTask::getEncryptionKey() const {
	return encryptionKey;
}

QByteArray const& KeyAndFixedNonceEncryptionCallbackTask::getUnencryptedData() const {
	return unencryptedData;
}

QByteArray const& KeyAndFixedNonceEncryptionCallbackTask::getEncryptedData() const {
	return encryptedData;
}

Nonce const& KeyAndFixedNonceEncryptionCallbackTask::getFixedNonce() const {
	return nonce;
}

void KeyAndFixedNonceEncryptionCallbackTask::taskRun() {
	LOGGER_DEBUG("Running KeyAndFixedNonceEncryptionCallbackTask for {} Bytes.", unencryptedData.size());
	try {
		this->encryptedData = cryptoBox->encryptForFixedNonce(unencryptedData, encryptionKey, nonce);

		finishedWithNoError();
	} catch (CryptoException& cryptoException) {
		finishedWithError(-1, QString(cryptoException.what()));
	}
}
