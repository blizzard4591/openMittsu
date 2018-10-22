#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Logging.h"

#include <QString>

namespace openmittsu {
	namespace tasks {

		KeyAndFixedNonceEncryptionCallbackTask::KeyAndFixedNonceEncryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, openmittsu::crypto::Nonce const& nonce) : MessageCallbackTask(message, acknowledgmentProcessor), m_cryptoBox(cryptoBox), m_unencryptedData(data), m_nonce(nonce), m_encryptionKey(), m_encryptedData() {
			// Intentionally left empty.
		}

		KeyAndFixedNonceEncryptionCallbackTask::~KeyAndFixedNonceEncryptionCallbackTask() {
			// Intentionally left empty.
		}

		openmittsu::crypto::EncryptionKey const& KeyAndFixedNonceEncryptionCallbackTask::getEncryptionKey() const {
			return m_encryptionKey;
		}

		QByteArray const& KeyAndFixedNonceEncryptionCallbackTask::getUnencryptedData() const {
			return m_unencryptedData;
		}

		QByteArray const& KeyAndFixedNonceEncryptionCallbackTask::getEncryptedData() const {
			return m_encryptedData;
		}

		openmittsu::crypto::Nonce const& KeyAndFixedNonceEncryptionCallbackTask::getFixedNonce() const {
			return m_nonce;
		}

		void KeyAndFixedNonceEncryptionCallbackTask::taskRun() {
			LOGGER_DEBUG("Running KeyAndFixedNonceEncryptionCallbackTask for {} Bytes.", m_unencryptedData.size());
			try {
				m_encryptedData = m_cryptoBox->encryptForFixedNonce(m_unencryptedData, m_encryptionKey, m_nonce);

				finishedWithNoError();
			} catch (openmittsu::exceptions::CryptoExceptionImpl& cryptoException) {
				finishedWithError(-1, QString(cryptoException.what()));
			}
		}

	}
}
