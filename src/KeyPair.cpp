#include "KeyPair.h"

#include "sodium.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "protocol/ProtocolSpecs.h"
#include "utility/Logging.h"
#include "pbkdf2-sha256.h"
#include "Base32.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <QByteArray>
#include <QString>
#include <QStringBuilder>

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
	throw InternalErrorException() << "Identity is not implemented for class KeyPair.";
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
		throw IllegalArgumentException() << "The supplied public key does not match the constructed public key!";
	}
	return kp;
}

QString KeyPair::toString() const {
	// Uses QStringBuilder syntax
#ifdef _MSC_VER
	QString result = "KeyPair(privKey = " % this->privateKey.toHex() % ", pubKey = " % this->publicKey.toHex() % ")";
#else
	QString result = QString("KeyPair(privKey = %1, pubKey = %2)").arg(QString(this->privateKey.toHex())).arg(QString(this->publicKey.toHex()));
#endif
	return result;
}

// Deprecated, do not use. Use the ClientConfiguration instead.
KeyPair KeyPair::fromBackup(QString const& base32Backup, QString const& password) {
	QByteArray decodedBase32 = Base32::decodeBase32Sequence(base32Backup);
	LOGGER_DEBUG("Base32: {}", QString(decodedBase32.toHex()).toStdString());
	if (decodedBase32.size() != BACKUP_DECODED_BYTES) {
		throw IllegalArgumentException() << "Size of decoded Backup String is incorrect (" << decodedBase32.size() << " Bytes instead of " << BACKUP_DECODED_BYTES << " Bytes).";
	}

	unsigned char encryptionKey[BACKUP_ENCRYPTION_KEY_BYTES + 1];
	sodium_memzero(encryptionKey, BACKUP_ENCRYPTION_KEY_BYTES + 1);

	// The pointer to the base32-decoded Backup
	unsigned char* decodedBase32Ptr = reinterpret_cast<unsigned char*>(decodedBase32.data());

	// The Salt used in the PBKDF2 Key Derivation process is embedded in the first 8 bytes of the Backup.
	QByteArray password8Bit = password.toUtf8();
	PKCS5_PBKDF2_HMAC(reinterpret_cast<unsigned char*>(password8Bit.data()), password8Bit.size(), decodedBase32Ptr, BACKUP_SALT_BYTES, BACKUP_KEY_PBKDF_ITERATIONS, BACKUP_ENCRYPTION_KEY_BYTES, encryptionKey);

	unsigned char nonceBytes[crypto_stream_NONCEBYTES];
	sodium_memzero(nonceBytes, crypto_stream_NONCEBYTES);

	crypto_stream_xor(&decodedBase32Ptr[BACKUP_SALT_BYTES], &decodedBase32Ptr[BACKUP_SALT_BYTES], BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES, nonceBytes, encryptionKey);

	// The last two bytes of the Backup contain the truncated SHA-256 Hash over the identity and its Private Key.
	unsigned char controlHash[crypto_hash_sha256_BYTES];
	sodium_memzero(controlHash, crypto_hash_sha256_BYTES);
	crypto_hash_sha256(controlHash, &(decodedBase32Ptr[BACKUP_SALT_BYTES]), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES);

	if (sodium_memcmp(&(decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES]), controlHash, BACKUP_HASH_BYTES) != 0) {
		throw IllegalArgumentException() << "Decryption of Backup failed: Invalid Control Hash (" << controlHash[0] << controlHash[1] << " vs. " << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES] << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + 1] << ").";
	}

	KeyPair kp;
	kp.privateKey = QByteArray(reinterpret_cast<char*>(&decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]), PROTO_KEY_LENGTH_BYTES);
	unsigned char derivedPublicKey[PROTO_KEY_LENGTH_BYTES];
	crypto_scalarmult_base(derivedPublicKey, &decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]);
	kp.publicKey = QByteArray(reinterpret_cast<char*>(derivedPublicKey), PROTO_KEY_LENGTH_BYTES);
	return kp;
}
