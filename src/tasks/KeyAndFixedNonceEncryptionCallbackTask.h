#ifndef OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/protocol/ContactId.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/EncryptionKey.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/MessageCallbackTask.h"

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

namespace openmittsu {
	namespace tasks {

		class KeyAndFixedNonceEncryptionCallbackTask : public MessageCallbackTask {
		public:
			KeyAndFixedNonceEncryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, openmittsu::crypto::Nonce const& nonce);
			virtual ~KeyAndFixedNonceEncryptionCallbackTask();

			openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
			QByteArray const& getUnencryptedData() const;

			QByteArray const& getEncryptedData() const;
			openmittsu::crypto::Nonce const& getFixedNonce() const;
		protected:
			virtual void taskRun() override;
		private:
			std::shared_ptr<openmittsu::crypto::FullCryptoBox> const m_cryptoBox;
			QByteArray const m_unencryptedData;
			openmittsu::crypto::Nonce const m_nonce;
			openmittsu::crypto::EncryptionKey const m_encryptionKey;
			QByteArray m_encryptedData;
		};

	}
}

#endif // OPENMITTSU_TASKS_KEYANDFIXEDNONCEENCRYPTIONCALLBACKTASK_H_
