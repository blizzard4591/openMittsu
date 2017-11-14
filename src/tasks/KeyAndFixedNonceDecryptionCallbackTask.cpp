#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Logging.h"

#include <QString>

namespace openmittsu {
	namespace tasks {

		KeyAndFixedNonceDecryptionCallbackTask::KeyAndFixedNonceDecryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, openmittsu::crypto::EncryptionKey const& encryptionKey, openmittsu::crypto::Nonce const& nonce) : MessageCallbackTask(message, acknowledgmentProcessor), m_cryptoBox(cryptoBox), m_encryptedData(encryptedData), m_nonce(nonce), m_encryptionKey(encryptionKey), m_decryptedData() {
			// Intentionally left empty.
		}

		KeyAndFixedNonceDecryptionCallbackTask::~KeyAndFixedNonceDecryptionCallbackTask() {
			// Intentionally left empty.
		}

		openmittsu::crypto::EncryptionKey const& KeyAndFixedNonceDecryptionCallbackTask::getEncryptionKey() const {
			return m_encryptionKey;
		}

		QByteArray const& KeyAndFixedNonceDecryptionCallbackTask::getDecryptedData() const {
			return m_decryptedData;
		}

		QByteArray const& KeyAndFixedNonceDecryptionCallbackTask::getEncryptedData() const {
			return m_encryptedData;
		}

		openmittsu::crypto::Nonce const& KeyAndFixedNonceDecryptionCallbackTask::getFixedNonce() const {
			return m_nonce;
		}

		void KeyAndFixedNonceDecryptionCallbackTask::taskRun() {
			LOGGER_DEBUG("Running KeyAndFixedNonceDecryptionCallbackTask for {} Bytes.", m_encryptedData.size());

			try {
				m_decryptedData = m_cryptoBox->decryptFromFixedNonce(m_encryptedData, m_encryptionKey, m_nonce);

				finishedWithNoError();
			} catch (openmittsu::exceptions::CryptoException& cryptoException) {
				finishedWithError(-1, QString(cryptoException.what()));
			}
		}

	}
}
