#include "src/messages/group/UnspecializedGroupMessage.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			UnspecializedGroupMessage::UnspecializedGroupMessage(FullMessageHeader const& messageHeader, GroupMessageContent* messageContent, QSet<openmittsu::protocol::ContactId> const& recipients) : Message(messageHeader, messageContent), m_recipients(recipients) {
				// Intentionally left empty.
			}

			UnspecializedGroupMessage::UnspecializedGroupMessage(PreliminaryGroupMessage const& message, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::PushFromId const& pushFromId) : Message(FullMessageHeader(message.getPreliminaryMessageHeader(), sender, pushFromId), message.getPreliminaryMessageContent()->clone()), m_recipients(message.getRecipients()) {
				// Intentionally left empty.
			}

			UnspecializedGroupMessage::UnspecializedGroupMessage(UnspecializedGroupMessage const& other) : Message(other.getMessageHeader(), other.getGroupMessageContent()->clone()), m_recipients(other.m_recipients) {
				// Intentionally left empty.
			}

			UnspecializedGroupMessage::~UnspecializedGroupMessage() {
				// Intentionally left empty.
			}

			GroupMessageContent const* UnspecializedGroupMessage::getGroupMessageContent() const {
				GroupMessageContent const * const gmc = dynamic_cast<GroupMessageContent const*>(getMessageContent());

				if (gmc == nullptr) {
					throw openmittsu::exceptions::InternalErrorException() << "This UnspecializedGroupMessage does not contain a GroupMessageContent?!";
				}

				return gmc;
			}

			Message* UnspecializedGroupMessage::withNewMessageContent(MessageContent* messageContent) const {
				if (dynamic_cast<GroupMessageContent*>(messageContent) == nullptr) {
					throw openmittsu::exceptions::IllegalArgumentException() << "Illegal Message content type, can not change from UnspecializedGroup- to Non-UnspecializedGroup message.";
				}
				return new UnspecializedGroupMessage(messageHeader, dynamic_cast<GroupMessageContent*>(messageContent), m_recipients);
			}

			QSet<openmittsu::protocol::ContactId> const& UnspecializedGroupMessage::getRecipients() const {
				return m_recipients;
			}

		}
	}
}

