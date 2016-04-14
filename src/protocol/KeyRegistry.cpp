#include "protocol/KeyRegistry.h"

#include "exceptions/CryptoException.h"
#include "exceptions/IllegalArgumentException.h"

KeyRegistry::KeyRegistry() {
	throw;
}

KeyRegistry::KeyRegistry(KeyRegistry const& other) : clientKeyPair(other.clientKeyPair), serverPublicKey(other.serverPublicKey), knownPublicKeys(other.knownPublicKeys), mutex() {
	// Intentionally left empty.
}

KeyRegistry::KeyRegistry(KeyPair const& clientKeyPair, PublicKey const& serverPublicKey, QHash<ContactId, PublicKey> const& preKnownPublicKeys) : clientKeyPair(clientKeyPair), serverPublicKey(serverPublicKey), knownPublicKeys(preKnownPublicKeys) {
	// Intentionally left empty.
}

KeyRegistry::~KeyRegistry() {
	// Intentionally left empty.
}

void KeyRegistry::addIdentity(ContactId const& identity, PublicKey const& publicKey) {
	if (hasIdentity(identity)) {
		PublicKey const& knownKey = knownPublicKeys.value(identity);
		if (knownKey == publicKey) {
			return;
		} else {
			throw CryptoException() << "Can not add public key for an identity which already has a different public key set.";
		}
	} else {
		mutex.lock();
		knownPublicKeys.insert(identity, publicKey);
		mutex.unlock();
	}
}

bool KeyRegistry::hasIdentity(ContactId const& identity) const {
	mutex.lock();
	bool result = knownPublicKeys.contains(identity);
	mutex.unlock();

	return result;
}

PublicKey KeyRegistry::getPublicKeyForIdentity(ContactId const& identity) const {
	if (!hasIdentity(identity)) {
		throw IllegalArgumentException() << "The identity given does not exist.";
	}

	mutex.lock();
	PublicKey result = knownPublicKeys.value(identity);
	mutex.unlock();

	return result;
}

KeyPair const& KeyRegistry::getClientLongTermKeyPair() const {
	return clientKeyPair;
}

PublicKey const& KeyRegistry::getServerLongTermPublicKey() const {
	return serverPublicKey;
}
