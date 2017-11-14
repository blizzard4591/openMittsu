#include "src/messages/contact/ReceiptMessageContent.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/ProtocolErrorException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/HexChar.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ReceiptMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_RECEIPT, new TypedMessageContentFactory<ReceiptMessageContent>());


			ReceiptMessageContent::ReceiptMessageContent() : ContactMessageContent(), messageIds(), receiptType(ReceiptType::RECEIVED) {
				// Only accessible and used by the MessageContentFactory.
			}

			ReceiptMessageContent::ReceiptMessageContent(std::vector<openmittsu::protocol::MessageId> const& relatedMessages, ReceiptType const& receiptType) : ContactMessageContent(), messageIds(relatedMessages), receiptType(receiptType) {
				// Intentionally left empty.
			}

			ReceiptMessageContent::~ReceiptMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ReceiptMessageContent::clone() const {
				return new ReceiptMessageContent(messageIds, receiptType);
			}

			ReceiptMessageContent::ReceiptType const& ReceiptMessageContent::getReceiptType() const {
				return receiptType;
			}

			std::vector<openmittsu::protocol::MessageId> const& ReceiptMessageContent::getReferredMessageIds() const {
				return messageIds;
			}

			MessageContent* ReceiptMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_RECEIPT, 2 + (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES), payload, false);

				int remainingBytes = payload.size() - 2;
				if ((remainingBytes % (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES)) != 0) {
					throw openmittsu::exceptions::IllegalArgumentException() << "Can not create MessageContent from payload with size " << payload.size() << " Bytes, which is not divisable by " << (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES);
				}

				ReceiptType type = charToReceiptType(payload.at(1));
				std::vector<openmittsu::protocol::MessageId> ids;

				int idCount = remainingBytes / (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES);
				int position = 2;
				for (int i = 0; i < idCount; ++i) {
					openmittsu::protocol::MessageId id(payload.mid(position, PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES));
					ids.push_back(id);

					position += (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES);
				}

				return new ReceiptMessageContent(ids, type);
			}

			QByteArray ReceiptMessageContent::toPacketPayload() const {
				QByteArray result(2, 0x00);

				result[0] = PROTO_MESSAGE_SIGNATURE_RECEIPT;
				result[1] = receiptTypeToChar(receiptType);

				for (openmittsu::protocol::MessageId id : messageIds) {
					result.append(id.getMessageIdAsByteArray());
				}

				return result;
			}

			char ReceiptMessageContent::receiptTypeToChar(ReceiptType const& receiptType) const {
				switch (receiptType) {
					case ReceiptType::AGREE:
						return PROTO_RECEIPT_TYPE_AGREE;
					case ReceiptType::DISAGREE:
						return PROTO_RECEIPT_TYPE_DISAGREE;
					case ReceiptType::RECEIVED:
						return PROTO_RECEIPT_TYPE_RECEIVED;
					case ReceiptType::SEEN:
						return PROTO_RECEIPT_TYPE_SEEN;
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Switch in ReceiptMessageContent::receiptTypeToChar is missing a case for a ReceiptType.";
						break;
				}
			}

			ReceiptMessageContent::ReceiptType ReceiptMessageContent::charToReceiptType(char c) const {
				switch (c) {
					case PROTO_RECEIPT_TYPE_AGREE:
						return ReceiptType::AGREE;
					case PROTO_RECEIPT_TYPE_DISAGREE:
						return ReceiptType::DISAGREE;
					case PROTO_RECEIPT_TYPE_RECEIVED:
						return ReceiptType::RECEIVED;
					case PROTO_RECEIPT_TYPE_SEEN:
						return ReceiptType::SEEN;
					default:
						throw openmittsu::exceptions::ProtocolErrorException() << "Received a receipt with unknown receipt type indicator 0x" << openmittsu::utility::HexChar(c) << ".";
				}
			}

		}
	}
}
