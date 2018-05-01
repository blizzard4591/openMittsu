#include "src/database/ContactMessageCursor.h"

#include "src/database/DatabaseWrapper.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>


namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		ContactMessageCursor::ContactMessageCursor(DatabaseWrapper const& database, openmittsu::protocol::ContactId const& contact) : openmittsu::dataproviders::messages::ContactMessageCursor(), m_database(database), m_contact(contact), m_messageId(0), m_isMessageIdValid(false), m_sortByValue(0), m_uid() {
			//
		}

		ContactMessageCursor::~ContactMessageCursor() {
			//
		}

		ContactMessageCursor::ContactMessageCursor(DatabaseWrapper const& database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) : openmittsu::dataproviders::messages::ContactMessageCursor(), m_database(database), m_contact(contact), m_messageId(messageId), m_isMessageIdValid(false), m_sortByValue(0), m_uid() {
			if (!seek(messageId)) {
				throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, invalid entry point.";
			}
		}

		openmittsu::protocol::ContactId const& ContactMessageCursor::getContactId() const {
			return m_contact;
		}

		std::shared_ptr<ContactMessage> ContactMessageCursor::getMessage() const {
			if (!isValid()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not create message wrapper for invalid message.";
			}
			return std::make_shared<DatabaseReadonlyContactMessage>(m_database.getContactMessage(m_contact, getMessageId()));
		}

		bool ContactMessageCursor::isValid() const {
			return m_isMessageIdValid;
		}

		bool ContactMessageCursor::seekToFirst() {
			m_database->g
			return getFirstOrLastMessageId(true);
		}

		bool ContactMessageCursor::seekToLast() {
			return getFirstOrLastMessageId(false);
		}

		virtual bool seek(openmittsu::protocol::MessageId const& messageId) override;
		virtual bool seekByUuid(QString const& uuid) override;

		virtual bool next() override;
		virtual bool previous() override;

		virtual openmittsu::protocol::MessageId const& getMessageId() const override;
		virtual QVector<QString> getLastMessages(std::size_t n) const override;

	}
}
