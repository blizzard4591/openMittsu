#ifndef OPENMITTSU_CRYPTO_ENCRYPTIONKEY_H_
#define OPENMITTSU_CRYPTO_ENCRYPTIONKEY_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>

#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace crypto {

		class EncryptionKey {
		public:
			EncryptionKey();
			EncryptionKey(QByteArray const& encryptionKeyBytes);
			EncryptionKey(EncryptionKey const& other);
			virtual ~EncryptionKey();

			QByteArray const& getEncryptionKey() const;
			unsigned char const* getEncryptionKeyAsCharPtr() const;

			static constexpr int getSizeOfEncryptionKeyInBytes() {
				return (PROTO_FILE_ENCRYPTION_KEY_LENGTH_BYTES);
			}

			bool operator ==(EncryptionKey const& other) const;
			bool operator !=(EncryptionKey const& other) const;
		private:
			QByteArray const encryptionKey;

			static QByteArray generateRandomEncryptionKey();
		};

	}
}

#endif // OPENMITTSU_CRYPTO_ENCRYPTIONKEY_H_
