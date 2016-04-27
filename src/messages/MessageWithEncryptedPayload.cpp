#include "messages/MessageWithEncryptedPayload.h"

#include "Pkcs7.h"
#include "exceptions/ProtocolErrorException.h"
#include "messages/MessageWithPayload.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/ProtocolSpecs.h"

MessageWithEncryptedPayload::MessageWithEncryptedPayload() : messageHeader(ContactId(0), MessageTime(0), ContactId(0), MessageId(0), MessageFlags(0x00), PushFromId(QStringLiteral(""))), nonce(), encryptedPayload() {
	throw;
}

MessageWithEncryptedPayload::MessageWithEncryptedPayload(FullMessageHeader const& messageHeader, Nonce const& nonce, QByteArray const& encryptedPayload) : messageHeader(messageHeader), nonce(nonce), encryptedPayload(encryptedPayload) {
	// Intentionally left empty.
}

MessageWithEncryptedPayload::MessageWithEncryptedPayload(MessageWithEncryptedPayload const& other) : messageHeader(other.messageHeader), nonce(other.nonce), encryptedPayload(other.encryptedPayload) {
	// Intentionally left empty.
}

MessageWithEncryptedPayload::~MessageWithEncryptedPayload() {
	// Intentionally left empty.
}

FullMessageHeader const& MessageWithEncryptedPayload::getMessageHeader() const {
	return messageHeader;
}

Nonce const& MessageWithEncryptedPayload::getNonce() const {
	return nonce;
}

QByteArray const& MessageWithEncryptedPayload::getEncryptedPayload() const {
	return encryptedPayload;
}

QByteArray MessageWithEncryptedPayload::toPacket() const {
	QByteArray result(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, 0x00);
	result[0] = (PROTO_PACKET_SIGNATURE_SENDING_MSG);
	
	result.append(messageHeader.toPacket());
	result.append(nonce.getNonce());
	result.append(encryptedPayload);

	return result;
}

MessageWithEncryptedPayload MessageWithEncryptedPayload::fromPacket(QByteArray const& packet) {
	if (packet.size() < (4 + FullMessageHeader::getFullMessageHeaderSize() + 1)) {
		throw ProtocolErrorException() << "Decoded message packet does not contain all necessary headers! Complete packet: " << QString(packet.toHex()).toStdString();
	}

	QByteArray const headerData = packet.mid(4, FullMessageHeader::getFullMessageHeaderSize());
	FullMessageHeader const fullMessageHeader(FullMessageHeader::fromHeaderData(headerData));

	Nonce const nonce(packet.mid(4 + FullMessageHeader::getFullMessageHeaderSize(), Nonce::getNonceLength()));

	QByteArray const encryptedPayload(packet.mid(4 + FullMessageHeader::getFullMessageHeaderSize() + Nonce::getNonceLength()));

	return MessageWithEncryptedPayload(fullMessageHeader, nonce, encryptedPayload);
}

MessageWithPayload MessageWithEncryptedPayload::decrypt(CryptoBox* cryptoBox) const {
	QByteArray decryptedPaddedPayload(cryptoBox->decrypt(encryptedPayload, nonce, messageHeader.getSender()));
	QByteArray payload(Pkcs7::decodePkcs7Sequence(decryptedPaddedPayload));

	return MessageWithPayload(getMessageHeader(), payload);
}
