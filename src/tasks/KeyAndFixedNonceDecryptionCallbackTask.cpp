#include "tasks/KeyAndFixedNonceDecryptionCallbackTask.h"

#include "exceptions/CryptoException.h"
#include "utility/Logging.h"

#include "protocol/ProtocolSpecs.h"
#include "protocol/NonceGenerator.h"

#include <QString>

KeyAndFixedNonceDecryptionCallbackTask::KeyAndFixedNonceDecryptionCallbackTask(CryptoBox* cryptoBox, Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, EncryptionKey const& encryptionKey, Nonce const& nonce) : MessageCallbackTask(message, acknowledgmentProcessor), cryptoBox(cryptoBox), encryptedData(encryptedData), nonce(nonce), encryptionKey(encryptionKey), decryptedData() {
	// Intentionally left empty.
}

KeyAndFixedNonceDecryptionCallbackTask::~KeyAndFixedNonceDecryptionCallbackTask() {
	// Intentionally left empty.
}

EncryptionKey const& KeyAndFixedNonceDecryptionCallbackTask::getEncryptionKey() const {
	return encryptionKey;
}

QByteArray const& KeyAndFixedNonceDecryptionCallbackTask::getDecryptedData() const {
	return decryptedData;
}

QByteArray const& KeyAndFixedNonceDecryptionCallbackTask::getEncryptedData() const {
	return encryptedData;
}

Nonce const& KeyAndFixedNonceDecryptionCallbackTask::getFixedNonce() const {
	return nonce;
}

void KeyAndFixedNonceDecryptionCallbackTask::taskRun() {
	LOGGER_DEBUG("Running KeyAndFixedNonceDecryptionCallbackTask for {} Bytes.", encryptedData.size());

	try {
		this->decryptedData = cryptoBox->decryptFromFixedNonce(encryptedData, encryptionKey, nonce);

		finishedWithNoError();
	} catch (CryptoException& cryptoException) {
		finishedWithError(-1, QString(cryptoException.what()));
	}
}
