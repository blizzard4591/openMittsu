#ifndef OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_
#define OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_

#include "messages/FullMessageHeader.h"
#include "messages/ContentType.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"

class MessageWithEncryptedPayload;

class MessageWithPayload {
public:
	MessageWithPayload(FullMessageHeader const& messageHeader, QByteArray const& payload);
	MessageWithPayload(MessageWithPayload const& other);
	virtual ~MessageWithPayload();

	FullMessageHeader const& getMessageHeader() const;
	QByteArray const& getPayload() const;

	virtual MessageWithEncryptedPayload encrypt(CryptoBox* cryptoBox) const;
private:
	FullMessageHeader const messageHeader;
	QByteArray const payload;

	// Disable the default constructor
	MessageWithPayload();
};

#endif // OPENMITTSU_MESSAGES_MESSAGEWITHPAYLOAD_H_
