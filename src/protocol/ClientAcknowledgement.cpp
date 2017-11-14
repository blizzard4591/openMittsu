#include "src/protocol/ClientAcknowledgement.h"

#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace protocol {

		ClientAcknowledgement::ClientAcknowledgement(ContactId const& sender, MessageId const& messageId) : sender(sender), messageId(messageId) {
			// Intentionally left empty.
		}

		ClientAcknowledgement::~ClientAcknowledgement() {
			// Intentionally left empty.
		}

		QByteArray ClientAcknowledgement::toPacket() const {
			// Header: 0x82 (Client Acknowledgment)
			QByteArray result(4, 0x00);
			result[0] = PROTO_PACKET_SIGNATURE_CLIENT_ACK;
			// Message Sender
			result.append(sender.getContactIdAsByteArray());
			// Message Id
			result.append(messageId.getMessageIdAsByteArray());

			return result;
		}

	}
}
