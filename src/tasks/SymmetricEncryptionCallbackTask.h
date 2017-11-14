#ifndef OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_
#define OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_

#include "src/messages/Message.h"
#include "src/protocol/ContactId.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/EncryptionKey.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/MessageCallbackTask.h"

#include <memory>

#include <QString>
#include <QByteArray>
#include <QSslCertificate>

namespace openmittsu {
	namespace tasks {

		class SymmetricEncryptionCallbackTask : public MessageCallbackTask {
		public:
			SymmetricEncryptionCallbackTask(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox, openmittsu::messages::Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, QByteArray const& data, openmittsu::protocol::ContactId const& symmetricKeyPartner);
			virtual ~SymmetricEncryptionCallbackTask();

			openmittsu::protocol::ContactId const& getSymmetricKeyPartner() const;
			QByteArray const& getUnencryptedData() const;

			QByteArray const& getEncryptedData() const;
			openmittsu::crypto::Nonce const& getNonce() const;
		protected:
			virtual void taskRun() override;
		private:
			std::shared_ptr<openmittsu::crypto::FullCryptoBox> const m_cryptoBox;
			QByteArray const m_unencryptedData;
			openmittsu::protocol::ContactId const m_symmetricKeyPartner;
			QByteArray m_encryptedData;
			openmittsu::crypto::Nonce m_nonce;
		};

	}
}

#endif // OPENMITTSU_TASKS_SYMMETRICENCRYPTIONCALLBACKTASK_H_
