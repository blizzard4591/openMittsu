#ifndef OPENMITTSU_PROTOCOL_KEYREGISTRY_H_
#define OPENMITTSU_PROTOCOL_KEYREGISTRY_H_

#include "PublicKey.h"
#include "KeyPair.h"
#include "protocol/ContactId.h"

#include <QMutex>
#include <QHash>

class KeyRegistry {
public:
	KeyRegistry(KeyPair const& clientKeyPair, PublicKey const& serverPublicKey, QHash<ContactId, PublicKey> const& preKnownPublicKeys);
	KeyRegistry(KeyRegistry const& other);
	virtual ~KeyRegistry();

	void addIdentity(ContactId const& identity, PublicKey const& publicKey);

	bool hasIdentity(ContactId const& identity) const;
	PublicKey getPublicKeyForIdentity(ContactId const& identity) const;

	KeyPair const& getClientLongTermKeyPair() const;
	PublicKey const& getServerLongTermPublicKey() const;
private:
	KeyRegistry();

	KeyPair clientKeyPair;
	PublicKey serverPublicKey;
	QHash<ContactId, PublicKey> knownPublicKeys;
	mutable QMutex mutex;
};

#endif // OPENMITTSU_PROTOCOL_KEYREGISTRY_H_
