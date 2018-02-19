#include "src/database/internal/DatabaseContactMessage.h"

#include "src/backup/ContactMessageBackupObject.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>
#include <chrono>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseContactMessage::DatabaseContactMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) : DatabaseMessage(database, messageId), DatabaseUserMessage(database, messageId), ContactMessage(), m_contact(contact) {
				if (!exists(database, contact, messageId)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, can not manipulate.";
				}
			}

			DatabaseContactMessage::~DatabaseContactMessage() {
				//
			}

			openmittsu::protocol::ContactId const& DatabaseContactMessage::getContactId() const {
				return m_contact;
			}

			ContactMessageType DatabaseContactMessage::getMessageType() const {
				return ContactMessageTypeHelper::fromString(queryField(QStringLiteral("contact_message_type")).toString());
			}

			QString DatabaseContactMessage::getContentAsText() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::TEXT) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as text because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return queryField(QStringLiteral("body")).toString();
			}

			openmittsu::utility::Location DatabaseContactMessage::getContentAsLocation() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::LOCATION) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as location because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return openmittsu::utility::Location::fromDatabaseString(queryField(QStringLiteral("body")).toString());
			}

			MediaFileItem DatabaseContactMessage::getContentAsImage() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::IMAGE) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as image because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return getMediaItem(getUid());
			}

		}
	}
}