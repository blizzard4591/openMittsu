#include "src/messages/FullMessageHeader.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/InternalErrorException.h"

#include "src/messages/PreliminaryMessageHeader.h"

#include "src/messages/contact/PreliminaryContactMessageHeader.h"
#include "src/messages/group/PreliminaryGroupMessageHeader.h"

#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace messages {

		FullMessageHeader::FullMessageHeader() : sender(0), receiver(0), messageId(0), time(openmittsu::protocol::MessageTime::now()), flags(), pushFromName(openmittsu::protocol::ContactId(0)) {
			throw;
		}

		FullMessageHeader::FullMessageHeader(openmittsu::protocol::ContactId const& receiverId, openmittsu::protocol::MessageTime const& time, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, MessageFlags const& flags, openmittsu::protocol::PushFromId const& pushFromId) : sender(sender), receiver(receiverId), messageId(messageId), time(time), flags(flags), pushFromName(pushFromId) {
			// Intentionally left empty.
		}

		FullMessageHeader::FullMessageHeader(contact::PreliminaryContactMessageHeader const* preliminaryHeader, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId) : sender(sender), receiver(preliminaryHeader->getReceiver()), messageId(preliminaryHeader->getMessageId()), time(preliminaryHeader->getTime()), flags(preliminaryHeader->getFlags()), pushFromName(pushFromId) {
			// Intentionally left empty.
		}

		FullMessageHeader::FullMessageHeader(group::PreliminaryGroupMessageHeader const* preliminaryHeader, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId) : sender(sender), receiver(openmittsu::protocol::ContactId(0)), messageId(preliminaryHeader->getMessageId()), time(preliminaryHeader->getTime()), flags(preliminaryHeader->getFlags()), pushFromName(pushFromId) {
			// Intentionally left empty.
		}

		FullMessageHeader::FullMessageHeader(FullMessageHeader const& messageHeader, openmittsu::protocol::ContactId const& replacementReceiver, openmittsu::protocol::MessageId const& replacementMessageId) : sender(messageHeader.sender), receiver(replacementReceiver), messageId(replacementMessageId), time(messageHeader.time), flags(messageHeader.flags), pushFromName(messageHeader.pushFromName) {
			// Intentionally left empty.
		}

		FullMessageHeader::FullMessageHeader(FullMessageHeader const& other) : sender(other.sender), receiver(other.receiver), messageId(other.messageId), time(other.time), flags(other.flags), pushFromName(other.pushFromName) {
			// Intentionally left empty.
		}

		FullMessageHeader::~FullMessageHeader() {
			// Intentionally left empty.
		}

		int FullMessageHeader::getFullMessageHeaderSize() {
			return (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES);
		}

		FullMessageHeader FullMessageHeader::fromHeaderData(QByteArray const& headerData) {
			if (headerData.size() != (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Size of Header data segment is " << headerData.size() << " instead of " << (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES) << " Bytes.";
			}

			int startPosition = 0;
			openmittsu::protocol::ContactId const senderId(headerData.mid(startPosition, PROTO_IDENTITY_LENGTH_BYTES));
			startPosition += PROTO_IDENTITY_LENGTH_BYTES;
			openmittsu::protocol::ContactId const receiverId(headerData.mid(startPosition, PROTO_IDENTITY_LENGTH_BYTES));
			startPosition += PROTO_IDENTITY_LENGTH_BYTES;
			openmittsu::protocol::MessageId const messageId(headerData.mid(startPosition, PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES));
			startPosition += PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES;
			openmittsu::protocol::MessageTime const timestamp(headerData.mid(startPosition, PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES));
			startPosition += PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES;
			MessageFlags const flags(headerData.at(startPosition));
			startPosition += (PROTO_MESSAGE_FLAGS_LENGTH_BYTES + PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES);
			openmittsu::protocol::PushFromId const pushFromId(headerData.mid(startPosition, PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES));
			startPosition += PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES;

			return FullMessageHeader(receiverId, timestamp, senderId, messageId, flags, pushFromId);
		}

		openmittsu::protocol::ContactId const& FullMessageHeader::getSender() const {
			return sender;
		}

		openmittsu::protocol::ContactId const& FullMessageHeader::getReceiver() const {
			if (receiver.getContactId() == 0) {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "Called getReceiver() on a FullMessageHeader which is used for an UnspecializedGroupMessage.";
			}

			return receiver;
		}

		openmittsu::protocol::MessageId const& FullMessageHeader::getMessageId() const {
			return messageId;
		}

		openmittsu::protocol::MessageTime const& FullMessageHeader::getTime() const {
			return time;
		}

		MessageFlags const& FullMessageHeader::getFlags() const {
			return flags;
		}

		openmittsu::protocol::PushFromId const& FullMessageHeader::getPushFromName() const {
			return pushFromName;
		}

		QByteArray FullMessageHeader::toPacket() const {
			QByteArray result;

			result.append(sender.getContactIdAsByteArray());
			result.append(receiver.getContactIdAsByteArray());
			result.append(messageId.getMessageIdAsByteArray());
			result.append(time.getMessageTimeAsByteArray());
			result.append(flags.getFlags());
			result.append(QByteArray(PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES, 0x00));
			result.append(pushFromName.getPushFromIdAsByteArray());

			if (result.size() != (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES)) {
				throw openmittsu::exceptions::InternalErrorException() << "Size of Header data segment is " << result.size() << " instead of " << (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES) << " Bytes.";
			}

			return result;
		}

	}
}
