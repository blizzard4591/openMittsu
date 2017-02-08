#include "protocol/NonceGenerator.h"

#include "exceptions/IllegalArgumentException.h"
#include "protocol/ProtocolSpecs.h"
#include "Endian.h"

#include <sodium.h>

NonceGenerator::NonceGenerator() : prefix(PROTO_NONCE_PREFIX_LENGTH_BYTES, 0x00), counter(0)  {
	// Generate a random Prefix
	randombytes_buf(prefix.data(), PROTO_NONCE_PREFIX_LENGTH_BYTES);
}

NonceGenerator::NonceGenerator(QByteArray const& noncePrefix) : prefix(noncePrefix), counter(0) {
	if (prefix.size() != (PROTO_NONCE_PREFIX_LENGTH_BYTES)) {
		throw IllegalArgumentException() << "Invalid nonce prefix with size of " << prefix.size() << " instead of " << (PROTO_NONCE_PREFIX_LENGTH_BYTES) << " Bytes.";
	}
}

NonceGenerator::~NonceGenerator() {
	// Intentionally left empty.
}

NonceGenerator& NonceGenerator::operator=(NonceGenerator const& other) {
	this->prefix = other.prefix;
	this->counter = other.counter;

	return *this;
}

Nonce NonceGenerator::getNextNonce() {	
	QByteArray result(prefix);

	mutex.lock();

	// Endian awareness
	const quint64 counterInLittleEndian = Endian::uint64FromHostEndianToLittleEndian(counter);

	++counter;

	mutex.unlock();

	// 64bit Integer, 8 Bytes
	result.append(reinterpret_cast<char const*>(&counterInLittleEndian), 8);


	return Nonce(result);
}

QByteArray const& NonceGenerator::getNoncePrefix() const {
	return prefix;
}

int NonceGenerator::getNonceLength() {
	return PROTO_MESSAGE_NONCE_LENGTH_BYTES;
}

int NonceGenerator::getNoncePrefixLength() {
	return PROTO_NONCE_PREFIX_LENGTH_BYTES;
}
