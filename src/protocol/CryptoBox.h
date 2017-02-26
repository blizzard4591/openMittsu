#ifndef OPENMITTSU_PROTOCOL_CRYPTOBOX_H_
#define OPENMITTSU_PROTOCOL_CRYPTOBOX_H_

#include <utility>
#include <QByteArray>

#include "protocol/ContactId.h"
#include "protocol/EncryptionKey.h"
#include "protocol/Nonce.h"
#include "protocol/NonceGenerator.h"
#include "protocol/KeyRegistry.h"
#include "PublicKey.h"
#include "KeyPair.h"

class AuthenticationPacket;

class CryptoBox {
public:
	CryptoBox(KeyRegistry const& keyRegistry);
	virtual ~CryptoBox();

	std::pair<Nonce, QByteArray> encrypt(QByteArray const& data, PublicKey const& targetIdentityPublicKey);
	std::pair<Nonce, QByteArray> encrypt(QByteArray const& data, ContactId const& targetIdentity);
	QByteArray decrypt(QByteArray const& encryptedData, Nonce const& nonce, ContactId const& sourceIdentity);

	QByteArray encryptForFixedNonce(QByteArray const& data, EncryptionKey const& encryptionKey, Nonce const& fixedNonce);
	QByteArray encryptForServer(QByteArray const& data);
	QByteArray encryptForServerWithLongTermKeys(QByteArray const& data, Nonce const& nonce);

	QByteArray decryptFromFixedNonce(QByteArray const& encryptedData, EncryptionKey const& encryptionKey, Nonce const& fixedNonce);
	QByteArray decryptFromServer(QByteArray const& encryptedData);
	QByteArray decryptFromServer(QByteArray const& encryptedData, PublicKey const& serverKey);

	KeyRegistry& getKeyRegistry();
	
	KeyPair const& getClientShortTermKeyPair() const;
	NonceGenerator const& getClientNonceGenerator() const;

	void setServerNoncePrefixFromServerHello(QByteArray const& newServerNonce);
	void setServerShortTermPublicKey(PublicKey const& newServerKey);

	friend class AuthenticationPacket;
private:
	KeyRegistry keyRegistry;

	KeyPair clientShortTermKey;
	PublicKey serverShortTermKey;
	NonceGenerator clientNonceGenerator;
	NonceGenerator serverNonceGenerator;
	
	PublicKey const& getServerShortTermPublicKey() const;
	NonceGenerator const& getServerNonceGenerator() const;

	QByteArray encrypt(QByteArray const& data, PublicKey const& pubKey, KeyPair const& privateKey, Nonce const& nonce) const;
};

#endif // OPENMITTSU_PROTOCOL_CRYPTOBOX_H_
