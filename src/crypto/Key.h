#ifndef OPENMITTSU_CRYPTO_KEY_H_
#define OPENMITTSU_CRYPTO_KEY_H_

#include "src/protocol/ProtocolSpecs.h"
#include "sodium.h"

#include <string>
#include <QByteArray>
#include <QString>

namespace openmittsu {
	namespace crypto {

		class Key {
		public:
			Key() {}
			virtual ~Key() {}

			virtual QByteArray const& getPrivateKey() const = 0;
			virtual QByteArray const& getPublicKey() const = 0;
			virtual QByteArray const& getIdentity() const = 0;

			virtual bool hasPrivateKey() const = 0;
			virtual bool hasPublicKey() const = 0;
			virtual bool hasIdentity() const = 0;

			virtual QString toQString() const = 0;
			virtual std::string toString() const = 0;

			static constexpr int getPrivateKeyLength() {
				return PROTO_KEY_LENGTH_BYTES;
			}

			static constexpr int getPublicKeyLength() {
				return PROTO_KEY_LENGTH_BYTES;
			}

			static constexpr int getIndentityLength() {
				return BACKUP_IDENTITY_BYTES;
			}

		};

	}
}

#endif // OPENMITTSU_CRYPTO_KEY_H_
