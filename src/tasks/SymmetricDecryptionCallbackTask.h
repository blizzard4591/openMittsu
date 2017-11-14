#ifndef OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_

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

		class SymmetricDecryptionCallbackTask : public MessageCallbackTask {
		public:
			SymmetricDecryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& encryptedData, openmittsu::crypto::Nonce const& nonce, openmittsu::protocol::ContactId const& symmetricKeyPartner);
			virtual ~SymmetricDecryptionCallbackTask();

			openmittsu::protocol::ContactId const& getSymmetricKeyPartner() const;
			QByteArray const& getDecryptedData() const;

			QByteArray const& getEncryptedData() const;
			openmittsu::crypto::Nonce const& getNonce() const;
		protected:
			virtual void taskRun() override;
		private:
			std::shared_ptr<openmittsu::crypto::FullCryptoBox> const m_cryptoBox;
			QByteArray const m_encryptedData;
			openmittsu::crypto::Nonce const m_nonce;
			openmittsu::protocol::ContactId const m_symmetricKeyPartner;
			QByteArray m_decryptedData;
		};

	}
}

#endif // OPENMITTSU_TASKS_SYMMETRICDECRYPTIONCALLBACKTASK_H_
