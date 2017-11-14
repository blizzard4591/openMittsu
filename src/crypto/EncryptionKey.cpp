#include "src/crypto/EncryptionKey.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/ByteArrayConversions.h"
#include <QHash>
#include <sodium.h>

namespace openmittsu {
	namespace crypto {

		EncryptionKey::EncryptionKey() : encryptionKey(generateRandomEncryptionKey()) {
			// Intentionally left empty.
		}

		EncryptionKey::EncryptionKey(QByteArray const& encryptionKeyBytes) : encryptionKey(encryptionKeyBytes) {
			if (encryptionKeyBytes.size() != getSizeOfEncryptionKeyInBytes()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Size of Encryption Key is " << encryptionKeyBytes.size() << " Bytes instead of " << getSizeOfEncryptionKeyInBytes() << " Bytes.";
			}
		}

		EncryptionKey::EncryptionKey(EncryptionKey const& other) : encryptionKey(other.encryptionKey) {
			// Intentionally left empty.
		}

		EncryptionKey::~EncryptionKey() {
			// Intentionally left empty.
		}

		QByteArray EncryptionKey::generateRandomEncryptionKey() {
			QByteArray data(getSizeOfEncryptionKeyInBytes(), 0x00);
			randombytes_buf(data.data(), getSizeOfEncryptionKeyInBytes());

			return data;
		}

		unsigned char const* EncryptionKey::getEncryptionKeyAsCharPtr() const {
			return reinterpret_cast<unsigned char const*>(encryptionKey.data());
		}

		QByteArray const& EncryptionKey::getEncryptionKey() const {
			return encryptionKey;
		}

		int EncryptionKey::getSizeOfEncryptionKeyInBytes() {
			return (PROTO_FILE_ENCRYPTION_KEY_LENGTH_BYTES);
		}

		bool EncryptionKey::operator ==(EncryptionKey const& other) const {
			return encryptionKey == other.encryptionKey;
		}

		bool EncryptionKey::operator !=(EncryptionKey const& other) const {
			return encryptionKey != other.encryptionKey;
		}

	}
}
