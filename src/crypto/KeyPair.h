#ifndef OPENMITTSU_CRYPTO_KEYPAIR_H_
#define OPENMITTSU_CRYPTO_KEYPAIR_H_

#include "src/protocol/ProtocolSpecs.h"
#include "src/crypto/PublicKey.h"
#include <QString>
#include <QByteArray>

namespace openmittsu {
	namespace crypto {

		class KeyPair : public PublicKey {
		public:
			KeyPair();
			KeyPair(KeyPair const& other);
			virtual ~KeyPair();

			virtual QByteArray const& getPrivateKey() const override;
			virtual QByteArray const& getIdentity() const override;

			virtual bool hasPrivateKey() const override;

			virtual QString toQString() const override;
			virtual std::string toString() const override;

			static KeyPair randomKey();
			static KeyPair fromArrays(unsigned char const* pubKey, unsigned char const* secKey);
		protected:
			QByteArray privateKey;
		};

	}
}

#endif // OPENMITTSU_CRYPTO_KEYPAIR_H_
