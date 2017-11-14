#include "src/tasks/SymmetricDecryptionCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Logging.h"

#include <QString>

namespace openmittsu {
	namespace tasks {

		SymmetricDecryptionCallbackTask::SymmetricDecryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, openmittsu::crypto::Nonce const& nonce, openmittsu::protocol::ContactId const& symmetricKeyPartner) : MessageCallbackTask(message, acknowledgmentProcessor), m_cryptoBox(cryptoBox), m_encryptedData(encryptedData), m_nonce(nonce), m_symmetricKeyPartner(symmetricKeyPartner), m_decryptedData() {
			// Intentionally left empty.
		}

		SymmetricDecryptionCallbackTask::~SymmetricDecryptionCallbackTask() {
			// Intentionally left empty.
		}

		openmittsu::protocol::ContactId const& SymmetricDecryptionCallbackTask::getSymmetricKeyPartner() const {
			return m_symmetricKeyPartner;
		}

		QByteArray const& SymmetricDecryptionCallbackTask::getDecryptedData() const {
			return m_decryptedData;
		}

		QByteArray const& SymmetricDecryptionCallbackTask::getEncryptedData() const {
			return m_encryptedData;
		}

		openmittsu::crypto::Nonce const& SymmetricDecryptionCallbackTask::getNonce() const {
			return m_nonce;
		}

		void SymmetricDecryptionCallbackTask::taskRun() {
			LOGGER_DEBUG("Running SymmetricDecryptionCallbackTask for {} Bytes from user {}.", m_encryptedData.size(), m_symmetricKeyPartner.toString());
			try {
				m_decryptedData = m_cryptoBox->decrypt(m_encryptedData, m_nonce, m_symmetricKeyPartner);

				finishedWithNoError();
			} catch (openmittsu::exceptions::CryptoException& cryptoException) {
				finishedWithError(-1, QString(cryptoException.what()));
			}
		}

	}
}
