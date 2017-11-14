#include "src/tasks/SymmetricEncryptionCallbackTask.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Logging.h"

#include <QString>

namespace openmittsu {
	namespace tasks {

		SymmetricEncryptionCallbackTask::SymmetricEncryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, openmittsu::protocol::ContactId const& symmetricKeyPartner) : MessageCallbackTask(message, acknowledgmentProcessor), m_cryptoBox(cryptoBox), m_unencryptedData(data), m_symmetricKeyPartner(symmetricKeyPartner), m_encryptedData(), m_nonce() {
			// Intentionally left empty.
		}

		SymmetricEncryptionCallbackTask::~SymmetricEncryptionCallbackTask() {
			// Intentionally left empty.
		}

		openmittsu::protocol::ContactId const& SymmetricEncryptionCallbackTask::getSymmetricKeyPartner() const {
			return m_symmetricKeyPartner;
		}

		QByteArray const& SymmetricEncryptionCallbackTask::getUnencryptedData() const {
			return m_unencryptedData;
		}

		QByteArray const& SymmetricEncryptionCallbackTask::getEncryptedData() const {
			return m_encryptedData;
		}

		openmittsu::crypto::Nonce const& SymmetricEncryptionCallbackTask::getNonce() const {
			return m_nonce;
		}

		void SymmetricEncryptionCallbackTask::taskRun() {
			LOGGER_DEBUG("Running SymmetricEncryptionCallbackTask for {} Bytes for user {}.", m_unencryptedData.size(), m_symmetricKeyPartner.toString());
			try {
				std::pair<openmittsu::crypto::Nonce, QByteArray> result = m_cryptoBox->encrypt(m_unencryptedData, m_symmetricKeyPartner);

				m_nonce = result.first;
				m_encryptedData = result.second;

				finishedWithNoError();
			} catch (openmittsu::exceptions::CryptoException& cryptoException) {
				finishedWithError(-1, QString(cryptoException.what()));
			}
		}

	}
}
