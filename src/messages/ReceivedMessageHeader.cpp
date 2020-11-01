#include "src/messages/ReceivedMessageHeader.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/InternalErrorException.h"

#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace messages {

		ReceivedMessageHeader::ReceivedMessageHeader() : sender(0), receiver(0), messageId(0), timeSent(openmittsu::protocol::MessageTime::now()), timeReceived(openmittsu::protocol::MessageTime::now()), flags(), pushFromName(openmittsu::protocol::ContactId(0)) {
			//
		}

		ReceivedMessageHeader::ReceivedMessageHeader(FullMessageHeader const& header) : sender(header.getSender()), receiver(header.getReceiver()), messageId(header.getMessageId()), timeSent(header.getTime()), timeReceived(openmittsu::protocol::MessageTime::now()), flags(header.getFlags()), pushFromName(header.getPushFromName()) {
			// Intentionally left empty.
		}

		ReceivedMessageHeader::ReceivedMessageHeader(FullMessageHeader const& header, openmittsu::protocol::MessageTime const& timeReceived) : sender(header.getSender()), receiver(header.getReceiver()), messageId(header.getMessageId()), timeSent(header.getTime()), timeReceived(timeReceived), flags(header.getFlags()), pushFromName(header.getPushFromName()) {
			// Intentionally left empty.
		}

		ReceivedMessageHeader::~ReceivedMessageHeader() {
			// Intentionally left empty.
		}

		openmittsu::protocol::ContactId const& ReceivedMessageHeader::getSender() const {
			return sender;
		}

		openmittsu::protocol::MessageId const& ReceivedMessageHeader::getMessageId() const {
			return messageId;
		}

		openmittsu::protocol::MessageTime const& ReceivedMessageHeader::getTimeSent() const {
			return timeSent;
		}

		openmittsu::protocol::MessageTime const& ReceivedMessageHeader::getTimeReceived() const {
			return timeReceived;
		}

		MessageFlags const& ReceivedMessageHeader::getFlags() const {
			return flags;
		}

		openmittsu::protocol::PushFromId const& ReceivedMessageHeader::getPushFromName() const {
			return pushFromName;
		}

	}
}
