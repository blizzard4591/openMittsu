#include "protocol/CryptoBox.h"

#include "exceptions/CryptoException.h"
#include "Endian.h"
#include <sodium.h>

CryptoBox::CryptoBox(KeyRegistry const& keyRegistry) : keyRegistry(keyRegistry), clientShortTermKey(KeyPair::randomKey()), serverShortTermKey(), clientNonceGenerator(), serverNonceGenerator() {
	// Intentionally left empty.
}

CryptoBox::~CryptoBox() {
	// Intentionally left empty.
}

std::pair<Nonce, QByteArray> CryptoBox::encrypt(QByteArray const& data, ContactId const& targetIdentity) {
	if (!keyRegistry.hasIdentity(targetIdentity)) {
		throw CryptoException() << "Can not encrypt for unknown identity.";
	}

	Nonce nonce;
	QByteArray encryptedData(data.size() + crypto_box_MACBYTES, 0x00);
	if (crypto_box_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(keyRegistry.getPublicKeyForIdentity(targetIdentity).getPublicKey().data()), reinterpret_cast<unsigned char const*>(keyRegistry.getClientLongTermKeyPair().getPrivateKey().data())) != 0) {
		throw CryptoException() << "Failed to encrypt data.";
	}

	return std::make_pair(nonce, encryptedData);
}

QByteArray CryptoBox::decrypt(QByteArray const& encryptedData, Nonce const& nonce, ContactId const& sourceIdentity) {
	if (!keyRegistry.hasIdentity(sourceIdentity)) {
		throw CryptoException() << "Can not decrypt from unknown identity.";
	} else if ((encryptedData.size() - crypto_box_MACBYTES) < 1) {
		throw CryptoException() << "Failed to decrypt data: Cipher text too short.";
	}

	QByteArray decryptedData(encryptedData.size() - crypto_box_MACBYTES, 0x00);

	if (crypto_box_open_easy(reinterpret_cast<unsigned char*>(decryptedData.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(keyRegistry.getPublicKeyForIdentity(sourceIdentity).getPublicKey().data()), reinterpret_cast<unsigned char const*>(keyRegistry.getClientLongTermKeyPair().getPrivateKey().data())) != 0) {
		throw CryptoException() << "Failed to decrypt data.";
	}

	return decryptedData;
}

KeyRegistry& CryptoBox::getKeyRegistry() {
	return keyRegistry;
}

NonceGenerator const& CryptoBox::getClientNonceGenerator() const {
	return clientNonceGenerator;
}

NonceGenerator const& CryptoBox::getServerNonceGenerator() const {
	return serverNonceGenerator;
}

void CryptoBox::setServerNoncePrefixFromServerHello(QByteArray const& newServerNoncePrefix) {
	serverNonceGenerator = NonceGenerator(newServerNoncePrefix);
}

KeyPair const& CryptoBox::getClientShortTermKeyPair() const {
	return clientShortTermKey;
}

PublicKey const& CryptoBox::getServerShortTermPublicKey() const {
	return serverShortTermKey;
}

void CryptoBox::setServerShortTermPublicKey(PublicKey const& newServerKey) {
	this->serverShortTermKey = newServerKey;
}

QByteArray CryptoBox::encryptForFixedNonce(QByteArray const& data, EncryptionKey const& encryptionKey, Nonce const& fixedNonce) {
	QByteArray encryptedData(data.size() + crypto_secretbox_MACBYTES, 0x00);
	if (crypto_secretbox_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), fixedNonce.getNonceAsCharPtr(), encryptionKey.getEncryptionKeyAsCharPtr()) != 0) {
		throw CryptoException() << "Failed to encrypt data for key and fixed nonce.";
	}

	return encryptedData;
}

QByteArray CryptoBox::encrypt(QByteArray const& data, PublicKey const& pubKey, KeyPair const& privateKey, Nonce const& nonce) const {
	QByteArray encryptedData(data.size() + crypto_box_MACBYTES, 0x00);
	if (crypto_box_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(pubKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(privateKey.getPrivateKey().data())) != 0) {
		throw CryptoException() << "Failed to encrypt data for server.";
	}

	return encryptedData;
}

QByteArray CryptoBox::encryptForServer(QByteArray const& data) {
	Nonce clientNonce(clientNonceGenerator.getNextNonce());

	return encrypt(data, serverShortTermKey, clientShortTermKey, clientNonce);
}

QByteArray CryptoBox::encryptForServerWithLongTermKeys(QByteArray const& data, Nonce const& nonce) {
	return encrypt(data, keyRegistry.getServerLongTermPublicKey(), keyRegistry.getClientLongTermKeyPair(), nonce);
}

QByteArray CryptoBox::decryptFromFixedNonce(QByteArray const& encryptedData, EncryptionKey const& encryptionKey, Nonce const& fixedNonce) {
	if ((encryptedData.size() - crypto_secretbox_MACBYTES) < 1) {
		throw CryptoException() << "Failed to decrypt data: Cipher text too short.";
	}
	
	QByteArray decryptedData(encryptedData.size() - crypto_secretbox_MACBYTES, 0x00);
	if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char*>(decryptedData.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), fixedNonce.getNonceAsCharPtr(), encryptionKey.getEncryptionKeyAsCharPtr()) != 0) {
		throw CryptoException() << "Failed to decrypt data for key and fixed nonce.";
	}

	return decryptedData;
}

QByteArray CryptoBox::decryptFromServer(QByteArray const& encryptedData, PublicKey const& serverKey) {
	if ((encryptedData.size() - crypto_box_MACBYTES) < 1) {
		throw CryptoException() << "Failed to decrypt data: Cipher text too short.";
	}

	Nonce const serverNonce(serverNonceGenerator.getNextNonce());
	QByteArray decodedPacket(encryptedData.size() - crypto_box_MACBYTES, 0x00);
	if (crypto_box_open_easy(reinterpret_cast<unsigned char*>(decodedPacket.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), serverNonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(serverKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(getClientShortTermKeyPair().getPrivateKey().data())) != 0) {
		throw CryptoException() << "Failed to decrypt data.";
	}

	return decodedPacket;
}

QByteArray CryptoBox::decryptFromServer(QByteArray const& encryptedData) {
	return decryptFromServer(encryptedData, serverShortTermKey);
}
