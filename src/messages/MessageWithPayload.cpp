#include "messages/MessageWithPayload.h"

#include "Pkcs7.h"
#include "exceptions/IllegalArgumentException.h"
#include "messages/MessageWithEncryptedPayload.h"
#include "protocol/ProtocolSpecs.h"

MessageWithPayload::MessageWithPayload() : messageHeader(ContactId(0), MessageTime(0), ContactId(0), MessageId(0), MessageFlags(0x00), PushFromId(QStringLiteral(""))), payload() {
	throw;
}

MessageWithPayload::MessageWithPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) : messageHeader(messageHeader), payload(payload) {
	if (payload.size() == 0) {
		throw IllegalArgumentException() << "The payload of a message can not be zero Bytes in size.";
	}
}

MessageWithPayload::MessageWithPayload(MessageWithPayload const& other) : messageHeader(other.messageHeader), payload(other.payload) {
	if (payload.size() == 0) {
		throw IllegalArgumentException() << "The payload of a message can not be zero Bytes in size.";
	}
}

MessageWithPayload::~MessageWithPayload() {
	// Intentionally left empty.
}

FullMessageHeader const& MessageWithPayload::getMessageHeader() const {
	return messageHeader;
}

QByteArray const& MessageWithPayload::getPayload() const {
	return payload;
}

MessageWithEncryptedPayload MessageWithPayload::encrypt(CryptoBox* cryptoBox) const {
	QByteArray paddedPayload(Pkcs7::encodePkcs7Sequence(payload));
	std::pair<Nonce, QByteArray> encryptionResult(cryptoBox->encrypt(paddedPayload, messageHeader.getReceiver()));
	
	return MessageWithEncryptedPayload(getMessageHeader(), encryptionResult.first, encryptionResult.second);
}
