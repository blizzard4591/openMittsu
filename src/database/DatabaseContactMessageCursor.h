#ifndef OPENMITTSU_DATABASE_DATABASECONTACTMESSAGECURSOR_H_
#define OPENMITTSU_DATABASE_DATABASECONTACTMESSAGECURSOR_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/DatabaseContactMessage.h"
#include "src/database/DatabaseMessageCursor.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"

namespace openmittsu {
	namespace database {
		class DatabaseContactMessageCursor : public virtual DatabaseMessageCursor, public virtual openmittsu::dataproviders::messages::ContactMessageCursor {
		public:
			explicit DatabaseContactMessageCursor(SimpleDatabase& database, openmittsu::protocol::ContactId const& contact);
			explicit DatabaseContactMessageCursor(SimpleDatabase& database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
			virtual ~DatabaseContactMessageCursor();

			virtual openmittsu::protocol::ContactId const& getContactId() const override;
			virtual std::shared_ptr<openmittsu::dataproviders::messages::ContactMessage> getMessage() const override;
		protected:
			virtual QString getWhereString() const override;
			virtual void bindWhereStringValues(QSqlQuery& query) const override;
			virtual QString getTableName() const override;
		private:
			openmittsu::protocol::ContactId const m_contact;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASECONTACTMESSAGECURSOR_H_
