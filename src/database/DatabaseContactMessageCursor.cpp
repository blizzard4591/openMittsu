#include "src/database/DatabaseContactMessageCursor.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>


namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseContactMessageCursor::DatabaseContactMessageCursor(SimpleDatabase& database, openmittsu::protocol::ContactId const& contact) : DatabaseMessageCursor(database), ContactMessageCursor(), m_contact(contact) {
			//
		}

		DatabaseContactMessageCursor::~DatabaseContactMessageCursor() {
			//
		}

		DatabaseContactMessageCursor::DatabaseContactMessageCursor(SimpleDatabase& database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) : DatabaseMessageCursor(database), ContactMessageCursor(), m_contact(contact) {
			if (!seek(messageId)) {
				throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, invalid entry point.";
			}
		}

		openmittsu::protocol::ContactId const& DatabaseContactMessageCursor::getContactId() const {
			return m_contact;
		}

		std::shared_ptr<ContactMessage> DatabaseContactMessageCursor::getMessage() const {
			if (!isValid()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not create message wrapper for invalid message.";
			}

			return std::make_shared<DatabaseContactMessage>(getDatabase(), m_contact, getMessageId());
		}

		QString DatabaseContactMessageCursor::getWhereString() const {
			return QStringLiteral("`identity` = :identity");
		}

		void DatabaseContactMessageCursor::bindWhereStringValues(QSqlQuery& query) const {
			query.bindValue(QStringLiteral(":identity"), QVariant(m_contact.toQString()));
		}

		QString DatabaseContactMessageCursor::getTableName() const {
			return QStringLiteral("contact_messages");
		}

	}
}
