#ifndef OPENMITTSU_DATABASE_CONTACTMESSAGECURSOR_H_
#define OPENMITTSU_DATABASE_CONTACTMESSAGECURSOR_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/internal/DatabaseContactMessage.h"
#include "src/database/internal/DatabaseMessageCursor.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"

namespace openmittsu {
	namespace database {
		class ContactMessageCursor : public virtual openmittsu::dataproviders::messages::ContactMessageCursor {
		public:
			ContactMessageCursor(DatabaseWrapper const& wrapper, openmittsu::protocol::ContactId const& contact);
			ContactMessageCursor(DatabaseWrapper const& wrapper, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
			virtual ~ContactMessageCursor();

			virtual openmittsu::protocol::ContactId const& getContactId() const override;
			virtual std::shared_ptr<openmittsu::dataproviders::messages::ContactMessage> getMessage() const override;
			virtual bool isValid() const override;
			virtual bool seekToFirst() override;
			virtual bool seekToLast() override;
			virtual bool seek(openmittsu::protocol::MessageId const& messageId) override;
			virtual bool seekByUuid(QString const& uuid) override;

			virtual bool next() override;
			virtual bool previous() override;

			virtual openmittsu::protocol::MessageId const& getMessageId() const override;
			virtual QVector<QString> getLastMessages(std::size_t n) const override;
		private:
			openmittsu::protocol::ContactId m_contact;
			openmittsu::protocol::MessageId m_messageId;
			bool m_isMessageIdValid;
			qint64 m_sortByValue;
			QString m_uid;
		};

	}
}

#endif // OPENMITTSU_DATABASE_CONTACTMESSAGECURSOR_H_
