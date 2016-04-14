#ifndef OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_
#define OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_

#include "messages/FullMessageHeader.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"

class MessageWithPayload;

class MessageWithEncryptedPayload {
public:
	MessageWithEncryptedPayload(FullMessageHeader const& messageHeader, Nonce const& nonce, QByteArray const& encryptedPayload);
	MessageWithEncryptedPayload(MessageWithEncryptedPayload const& other);
	virtual ~MessageWithEncryptedPayload();

	FullMessageHeader const& getMessageHeader() const;
	Nonce const& getNonce() const;
	QByteArray const& getEncryptedPayload() const;

	virtual QByteArray toPacket() const;
	virtual MessageWithPayload decrypt(CryptoBox* cryptoBox) const;

	static MessageWithEncryptedPayload fromPacket(QByteArray const& packet);
private:
	FullMessageHeader const messageHeader;
	Nonce const nonce;
	QByteArray const encryptedPayload;

	// Disable the default constructor
	MessageWithEncryptedPayload();
};

#endif // OPENMITTSU_MESSAGES_MESSAGEWITHENCRYPTEDPAYLOAD_H_
