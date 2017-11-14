#ifndef OPENMITTSU_CRYPTO_PUBLICKEY_H_
#define OPENMITTSU_CRYPTO_PUBLICKEY_H_

#include "src/protocol/ProtocolSpecs.h"
#include "src/crypto/Key.h"
#include <QString>
#include <QByteArray>
#include <QMetaType>

namespace openmittsu {
	namespace crypto {

		class PublicKey : public Key {
		public:
			PublicKey();
			PublicKey(PublicKey const& other);
			virtual ~PublicKey();

			virtual QByteArray const& getPrivateKey() const override;
			virtual QByteArray const& getPublicKey() const override;
			virtual QByteArray const& getIdentity() const override;

			virtual bool hasPrivateKey() const override;
			virtual bool hasPublicKey() const override;
			virtual bool hasIdentity() const override;

			virtual QByteArray const& getFingerprint() const;
			virtual QByteArray const& getFullFingerprint() const;

			virtual QString toQString() const override;
			virtual std::string toString() const override;

			static PublicKey fromDecodedServerResponse(QByteArray const& response);
			static PublicKey fromHexString(QString const& hexString);

			bool operator ==(PublicKey const& other) const;
			bool operator !=(PublicKey const& other) const;
		protected:
			QByteArray publicKey;
			QByteArray halfFingerprint;
			QByteArray fullFingerprint;
			void calculateFingerprintFromPublicKey();
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::crypto::PublicKey)

#endif // OPENMITTSU_CRYPTO_PUBLICKEY_H_
