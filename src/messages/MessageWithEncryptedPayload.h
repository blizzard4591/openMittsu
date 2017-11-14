#ifndef OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_
#define OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_

#include "src/messages/FullMessageHeader.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"

#include <memory>

namespace openmittsu {
	namespace messages {

		class MessageWithPayload;

		class MessageWithEncryptedPayload {
		public:
			MessageWithEncryptedPayload(FullMessageHeader const& messageHeader, openmittsu::crypto::Nonce const& nonce, QByteArray const& encryptedPayload);
			MessageWithEncryptedPayload(MessageWithEncryptedPayload const& other);
			virtual ~MessageWithEncryptedPayload();

			FullMessageHeader const& getMessageHeader() const;
			openmittsu::crypto::Nonce const& getNonce() const;
			QByteArray const& getEncryptedPayload() const;

			virtual QByteArray toPacket() const;
			virtual MessageWithPayload decrypt(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const;

			static MessageWithEncryptedPayload fromPacket(QByteArray const& packet);
		private:
			FullMessageHeader const messageHeader;
			openmittsu::crypto::Nonce const nonce;
			QByteArray const encryptedPayload;

			// Disable the default constructor
			MessageWithEncryptedPayload();
		};

	}
}

#endif // OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_
