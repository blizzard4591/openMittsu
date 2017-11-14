#ifndef OPENMITTSU_CRYPTO_FULLCRYPTOBOX_H_
#define OPENMITTSU_CRYPTO_FULLCRYPTOBOX_H_

#include <utility>
#include <QByteArray>

#include "src/protocol/ContactId.h"
#include "src/crypto/BasicCryptoBox.h"
#include "src/crypto/EncryptionKey.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/NonceGenerator.h"
#include "src/dataproviders/KeyRegistry.h"
#include "src/crypto/PublicKey.h"
#include "src/crypto/KeyPair.h"

namespace openmittsu {
	namespace protocol {
		class AuthenticationPacket;
	}

	namespace crypto {

		class FullCryptoBox : public BasicCryptoBox {
		public:
			FullCryptoBox(openmittsu::dataproviders::KeyRegistry const& keyRegistry);
			FullCryptoBox(FullCryptoBox&& other) = default;

			virtual ~FullCryptoBox();

			using BasicCryptoBox::encrypt;
			virtual std::pair<Nonce, QByteArray> encrypt(QByteArray const& data, openmittsu::protocol::ContactId const& targetIdentity);
			QByteArray decrypt(QByteArray const& encryptedData, openmittsu::crypto::Nonce const& nonce, openmittsu::protocol::ContactId const& sourceIdentity);

			QByteArray encryptForServer(QByteArray const& data);

			QByteArray decryptFromServer(QByteArray const& encryptedData);
			QByteArray decryptFromServer(QByteArray const& encryptedData, openmittsu::crypto::PublicKey const& serverKey);

			openmittsu::dataproviders::KeyRegistry& getKeyRegistry();

			openmittsu::crypto::KeyPair const& getClientShortTermKeyPair() const;
			openmittsu::crypto::NonceGenerator const& getClientNonceGenerator() const;

			void setServerNoncePrefixFromServerHello(QByteArray const& newServerNonce);
			void setServerShortTermPublicKey(PublicKey const& newServerKey);

			friend class openmittsu::protocol::AuthenticationPacket;
		private:
			openmittsu::dataproviders::KeyRegistry m_keyRegistry;

			openmittsu::crypto::KeyPair m_clientShortTermKey;
			openmittsu::crypto::PublicKey m_serverShortTermKey;
			openmittsu::crypto::NonceGenerator m_clientNonceGenerator;
			openmittsu::crypto::NonceGenerator m_serverNonceGenerator;

			openmittsu::crypto::PublicKey const& getServerShortTermPublicKey() const;
			openmittsu::crypto::NonceGenerator const& getServerNonceGenerator() const;

			QByteArray encrypt(QByteArray const& data, openmittsu::crypto::PublicKey const& pubKey, openmittsu::crypto::KeyPair const& privateKey, openmittsu::crypto::Nonce const& nonce) const;
		};

	}
}

#endif // OPENMITTSU_CRYPTO_FULLCRYPTOBOX_H_
