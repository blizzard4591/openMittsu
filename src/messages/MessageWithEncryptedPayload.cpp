#include "src/messages/MessageWithEncryptedPayload.h"

#include "src/encoding/Pkcs7.h"
#include "src/exceptions/ProtocolErrorException.h"
#include "src/messages/MessageWithPayload.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace messages {

		MessageWithEncryptedPayload::MessageWithEncryptedPayload() : messageHeader(openmittsu::protocol::ContactId(0), openmittsu::protocol::MessageTime(0), openmittsu::protocol::ContactId(0), openmittsu::protocol::MessageId(0), MessageFlags(0x00), openmittsu::protocol::PushFromId(QStringLiteral(""))), nonce(), encryptedPayload() {
			throw;
		}

		MessageWithEncryptedPayload::MessageWithEncryptedPayload(FullMessageHeader const& messageHeader, openmittsu::crypto::Nonce const& nonce, QByteArray const& encryptedPayload) : messageHeader(messageHeader), nonce(nonce), encryptedPayload(encryptedPayload) {
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

		openmittsu::crypto::Nonce const& MessageWithEncryptedPayload::getNonce() const {
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
				throw openmittsu::exceptions::ProtocolErrorException() << "Decoded message packet does not contain all necessary headers! Complete packet: " << QString(packet.toHex()).toStdString();
			}

			QByteArray const headerData = packet.mid(4, FullMessageHeader::getFullMessageHeaderSize());
			FullMessageHeader const fullMessageHeader(FullMessageHeader::fromHeaderData(headerData));

			openmittsu::crypto::Nonce const nonce(packet.mid(4 + FullMessageHeader::getFullMessageHeaderSize(), openmittsu::crypto::Nonce::getNonceLength()));

			QByteArray const encryptedPayload(packet.mid(4 + FullMessageHeader::getFullMessageHeaderSize() + openmittsu::crypto::Nonce::getNonceLength()));

			return MessageWithEncryptedPayload(fullMessageHeader, nonce, encryptedPayload);
		}

		MessageWithPayload MessageWithEncryptedPayload::decrypt(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
			QByteArray decryptedPaddedPayload(cryptoBox->decrypt(encryptedPayload, nonce, messageHeader.getSender()));
			QByteArray payload(openmittsu::encoding::Pkcs7::decodePkcs7Sequence(decryptedPaddedPayload));

			return MessageWithPayload(getMessageHeader(), payload);
		}

	}
}
