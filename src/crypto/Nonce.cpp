#include "src/crypto/Nonce.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include <sodium.h>

namespace openmittsu {
	namespace crypto {

		Nonce::Nonce() : nonce(PROTO_MESSAGE_NONCE_LENGTH_BYTES, 0x00) {
			randombytes_buf(nonce.data(), PROTO_MESSAGE_NONCE_LENGTH_BYTES);
		}

		Nonce::Nonce(QByteArray const& nonceData) : nonce(nonceData) {
			if (nonce.size() != (PROTO_MESSAGE_NONCE_LENGTH_BYTES)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Invalid nonce size of " << nonce.size() << " instead of " << (PROTO_MESSAGE_NONCE_LENGTH_BYTES) << " Bytes.";
			}
		}

		Nonce::~Nonce() {
			// Intentionally left empty.
		}

		QByteArray const& Nonce::getNonce() const {
			return nonce;
		}

		unsigned char const* Nonce::getNonceAsCharPtr() const {
			return reinterpret_cast<unsigned char const*>(nonce.data());
		}

		int Nonce::getNonceLength() {
			return (PROTO_MESSAGE_NONCE_LENGTH_BYTES);
		}

	}
}
