#include "src/crypto/PublicKey.h"

#include "sodium.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"

#include <cstring>

namespace openmittsu {
	namespace crypto {

		PublicKey::PublicKey() : Key(), publicKey(PROTO_KEY_LENGTH_BYTES, 0x00) {
			// All-zero key
			calculateFingerprintFromPublicKey();
		}

		PublicKey::PublicKey(PublicKey const& other) : Key(), publicKey(other.publicKey) {
			//
			calculateFingerprintFromPublicKey();
		}

		PublicKey::~PublicKey() {
			//
		}

		QByteArray const& PublicKey::getPrivateKey() const {
			throw openmittsu::exceptions::InternalErrorException() << "Private Key is not implemented for class PublicKey.";
		}

		QByteArray const& PublicKey::getPublicKey() const {
			return publicKey;
		}

		QByteArray const& PublicKey::getIdentity() const {
			throw openmittsu::exceptions::InternalErrorException() << "Identity is not implemented for class PublicKey.";
		}

		QByteArray const& PublicKey::getFingerprint() const {
			return halfFingerprint;
		}

		QByteArray const& PublicKey::getFullFingerprint() const {
			return fullFingerprint;
		}

		bool PublicKey::hasPrivateKey() const {
			return false;
		}

		bool PublicKey::hasPublicKey() const {
			return true;
		}

		bool PublicKey::hasIdentity() const {
			return false;
		}

		PublicKey PublicKey::fromDecodedServerResponse(QByteArray const& response) {
			PublicKey pk;
			pk.publicKey = response.left(Key::getPublicKeyLength());
			pk.calculateFingerprintFromPublicKey();
			return pk;
		}

		PublicKey PublicKey::fromHexString(QString const& hexString) {
			PublicKey pk;
			pk.publicKey = QByteArray::fromHex(hexString.toLatin1());
			pk.calculateFingerprintFromPublicKey();
			return pk;
		}

		QString PublicKey::toQString() const {
			return QStringLiteral("PublicKey(pubKey = %1)").arg(QString(this->publicKey.toHex()));
		}

		std::string PublicKey::toString() const {
			return toQString().toStdString();
		}

		void PublicKey::calculateFingerprintFromPublicKey() {
			QByteArray hash(crypto_hash_sha256_BYTES, 0x00);
			crypto_hash_sha256(reinterpret_cast<unsigned char*>(hash.data()), reinterpret_cast<unsigned char const*>(publicKey.data()), PROTO_KEY_LENGTH_BYTES);
			halfFingerprint = hash.left(PROTO_FINGERPRINT_LENGTH_BYTES);
			fullFingerprint = hash.left(2 * PROTO_FINGERPRINT_LENGTH_BYTES);
		}

		bool PublicKey::operator ==(PublicKey const& other) const {
			return this->publicKey == other.publicKey;
		}

		bool PublicKey::operator !=(PublicKey const& other) const {
			return this->publicKey != other.publicKey;
		}

	}
}
