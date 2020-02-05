#ifndef OPENMITTSU_CRYPTO_NONCEGENERATOR_H_
#define OPENMITTSU_CRYPTO_NONCEGENERATOR_H_

#include "src/crypto/Nonce.h"
#include <QByteArray>
#include <QMutex>

namespace openmittsu {
	namespace crypto {

		class NonceGenerator {
		public:
			NonceGenerator();
			NonceGenerator(NonceGenerator&& other);
			NonceGenerator(QByteArray const& noncePrefix);
			virtual ~NonceGenerator();

			NonceGenerator& operator=(NonceGenerator const& other);

			QByteArray const& getNoncePrefix() const;
			Nonce getNextNonce();

			static constexpr int getNonceLength() {
				return PROTO_MESSAGE_NONCE_LENGTH_BYTES;
			}
			static constexpr int getNoncePrefixLength() {
				return PROTO_NONCE_PREFIX_LENGTH_BYTES;
			}
		private:
			mutable QMutex m_mutex;

			QByteArray m_prefix;
			quint64 m_counter;
		};

	}
}

#endif // OPENMITTSU_CRYPTO_NONCEGENERATOR_H_
