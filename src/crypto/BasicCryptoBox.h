#ifndef OPENMITTSU_CRYPTO_BASICCRYPTOBOX_H_
#define OPENMITTSU_CRYPTO_BASICCRYPTOBOX_H_

#include <utility>
#include <QByteArray>

#include "src/protocol/ContactId.h"
#include "src/crypto/EncryptionKey.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/NonceGenerator.h"
#include "src/crypto/SafeMasterKey.h"
#include "src/dataproviders/KeyRegistry.h"
#include "src/crypto/PublicKey.h"
#include "src/crypto/KeyPair.h"

namespace openmittsu {
	namespace crypto {

		class BasicCryptoBox {
		public:
			BasicCryptoBox(openmittsu::crypto::KeyPair const& clientLongTermKey, openmittsu::crypto::PublicKey const& serverLongTermKey);
			BasicCryptoBox(BasicCryptoBox const& other);
			virtual ~BasicCryptoBox();

			virtual std::pair<Nonce, QByteArray> encrypt(QByteArray const& data, openmittsu::crypto::PublicKey const& targetIdentityPublicKey);

			virtual QByteArray encryptForFixedNonce(QByteArray const& data, openmittsu::crypto::EncryptionKey const& encryptionKey, openmittsu::crypto::Nonce const& fixedNonce);
			virtual QByteArray encryptForServerWithLongTermKeys(QByteArray const& data, openmittsu::crypto::Nonce const& nonce);

			virtual QByteArray decryptFromFixedNonce(QByteArray const& encryptedData, openmittsu::crypto::EncryptionKey const& encryptionKey, openmittsu::crypto::Nonce const& fixedNonce);
		protected:
			openmittsu::crypto::KeyPair const m_clientLongTermKey;
			openmittsu::crypto::PublicKey const m_serverLongTermKey;

			QByteArray encrypt(QByteArray const& data, openmittsu::crypto::PublicKey const& pubKey, openmittsu::crypto::KeyPair const& privateKey, openmittsu::crypto::Nonce const& nonce) const;
		};

	}
}

#endif // OPENMITTSU_CRYPTO_BASICCRYPTOBOX_H_
