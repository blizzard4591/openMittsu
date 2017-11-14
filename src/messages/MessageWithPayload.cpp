#include "src/messages/MessageWithPayload.h"

#include "src/encoding/Pkcs7.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace messages {

		MessageWithPayload::MessageWithPayload() : messageHeader(openmittsu::protocol::ContactId(0), openmittsu::protocol::MessageTime(0), openmittsu::protocol::ContactId(0), openmittsu::protocol::MessageId(0), MessageFlags(0x00), openmittsu::protocol::PushFromId(QStringLiteral(""))), payload() {
			throw;
		}

		MessageWithPayload::MessageWithPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) : messageHeader(messageHeader), payload(payload) {
			if (payload.size() == 0) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The payload of a message can not be zero Bytes in size.";
			}
		}

		MessageWithPayload::MessageWithPayload(MessageWithPayload const& other) : messageHeader(other.messageHeader), payload(other.payload) {
			if (payload.size() == 0) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The payload of a message can not be zero Bytes in size.";
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

		MessageWithEncryptedPayload MessageWithPayload::encrypt(std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
			QByteArray paddedPayload(openmittsu::encoding::Pkcs7::encodePkcs7Sequence(payload));
			std::pair<openmittsu::crypto::Nonce, QByteArray> encryptionResult(cryptoBox->encrypt(paddedPayload, messageHeader.getReceiver()));

			return MessageWithEncryptedPayload(getMessageHeader(), encryptionResult.first, encryptionResult.second);
		}

	}
}
