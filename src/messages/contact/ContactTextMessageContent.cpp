#include "src/messages/contact/ContactTextMessageContent.h"

#include "src/messages/MessageContentRegistry.h"
#include "src/protocol/ProtocolSpecs.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactTextMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_TEXT, new TypedMessageContentFactory<ContactTextMessageContent>());

			ContactTextMessageContent::ContactTextMessageContent() : ContactMessageContent(), text() {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactTextMessageContent::ContactTextMessageContent(QString const& text) : ContactMessageContent(), text(text) {
				// Intentionally left empty.
			}

			ContactTextMessageContent::~ContactTextMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactTextMessageContent::clone() const {
				return new ContactTextMessageContent(text);
			}

			QString const& ContactTextMessageContent::getText() const {
				return text;
			}

			MessageContent* ContactTextMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_TEXT, 2, payload);

				QString payloadText(QString::fromUtf8(payload.mid(1)));

				return new ContactTextMessageContent(payloadText);
			}

			QByteArray ContactTextMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_TEXT);
				result.append(text.toUtf8());

				return result;
			}

		}
	}
}
