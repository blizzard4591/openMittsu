#include "src/crypto/KeyPair.h"

#include "sodium.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/InvalidInputException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/Logging.h"
#include "src/crypto/pbkdf2-sha256.h"
#include "src/encoding/Base32.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <QByteArray>
#include <QString>

namespace openmittsu {
	namespace crypto {

		KeyPair::KeyPair() : PublicKey(), privateKey(PROTO_KEY_LENGTH_BYTES, 0x00) {
			// All-zero keypair
		}

		KeyPair::KeyPair(KeyPair const& other) : PublicKey(other), privateKey(other.privateKey) {
			//
		}

		KeyPair::~KeyPair() {
			//
		}

		QByteArray const& KeyPair::getPrivateKey() const {
			return privateKey;
		}

		QByteArray const& KeyPair::getIdentity() const {
			throw openmittsu::exceptions::InternalErrorException() << "Identity is not implemented for class KeyPair.";
		}

		bool KeyPair::hasPrivateKey() const {
			return true;
		}

		KeyPair KeyPair::randomKey() {
			KeyPair kp;
			// Init with a random key
			crypto_box_keypair(reinterpret_cast<unsigned char*>(kp.publicKey.data()), reinterpret_cast<unsigned char*>(kp.privateKey.data()));

			return kp;
		}

		KeyPair KeyPair::fromArrays(unsigned char const* pubKey, unsigned char const* secKey) {
			KeyPair kp;
			kp.privateKey = QByteArray(reinterpret_cast<char const*>(secKey), PROTO_KEY_LENGTH_BYTES);
			kp.publicKey = QByteArray(reinterpret_cast<char const*>(pubKey), PROTO_KEY_LENGTH_BYTES);
			unsigned char testPubKey[32];
			crypto_scalarmult_base(testPubKey, secKey);
			if (sodium_memcmp(testPubKey, pubKey, 32) != 0) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The supplied public key does not match the constructed public key!";
			}
			return kp;
		}

		QString KeyPair::toQString() const {
			return QStringLiteral("KeyPair(privKey = %1, pubKey = %2)").arg(QString(this->privateKey.toHex())).arg(QString(this->publicKey.toHex()));
		}

		std::string KeyPair::toString() const {
			return toQString().toStdString();
		}

	}
}
