#include "src/crypto/NonceGenerator.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/Endian.h"

#include <sodium.h>

namespace openmittsu {
	namespace crypto {

		NonceGenerator::NonceGenerator() : m_mutex(), m_prefix(PROTO_NONCE_PREFIX_LENGTH_BYTES, 0x00), m_counter(1) {
			// Generate a random Prefix
			randombytes_buf(m_prefix.data(), PROTO_NONCE_PREFIX_LENGTH_BYTES);
		}
		
		NonceGenerator::NonceGenerator(NonceGenerator&& other) : m_mutex() {
			QMutexLocker lock(&other.m_mutex);
			m_prefix = std::move(other.m_prefix);
			m_counter = std::move(other.m_counter);
		}

		NonceGenerator::NonceGenerator(QByteArray const& noncePrefix) : m_mutex(), m_prefix(noncePrefix), m_counter(1) {
			if (m_prefix.size() != (PROTO_NONCE_PREFIX_LENGTH_BYTES)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Invalid nonce prefix with size of " << m_prefix.size() << " instead of " << (PROTO_NONCE_PREFIX_LENGTH_BYTES) << " Bytes.";
			}
		}

		NonceGenerator::~NonceGenerator() {
			// Intentionally left empty.
		}

		NonceGenerator& NonceGenerator::operator=(NonceGenerator const& other) {
			QMutexLocker lockOther(&other.m_mutex);
			QMutexLocker lock(&m_mutex);

			this->m_prefix = other.m_prefix;
			this->m_counter = other.m_counter;

			return *this;
		}

		Nonce NonceGenerator::getNextNonce() {
			QByteArray result(m_prefix);

			QMutexLocker lock(&m_mutex);

			// Endian awareness
			const quint64 counterInLittleEndian = openmittsu::utility::Endian::uint64FromHostEndianToLittleEndian(m_counter);

			++m_counter;

			// 64bit Integer, 8 Bytes
			result.append(reinterpret_cast<char const*>(&counterInLittleEndian), 8);


			return Nonce(result);
		}

		QByteArray const& NonceGenerator::getNoncePrefix() const {
			return m_prefix;
		}

	}
}
