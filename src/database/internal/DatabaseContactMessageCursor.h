#ifndef OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGECURSOR_H_
#define OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGECURSOR_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/DatabaseReadonlyContactMessage.h"
#include "src/database/internal/DatabaseContactMessage.h"
#include "src/database/internal/DatabaseMessageCursor.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"

namespace openmittsu {
	namespace database {
		namespace internal {
			class DatabaseContactMessageCursor : public virtual DatabaseMessageCursor, public virtual openmittsu::dataproviders::messages::ContactMessageCursor {
			public:
				explicit DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact);
				explicit DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
				explicit DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, QString const& uuid);
				virtual ~DatabaseContactMessageCursor();

				virtual openmittsu::protocol::ContactId const& getContactId() const override;
				virtual std::shared_ptr<openmittsu::dataproviders::messages::ContactMessage> getMessage() const override;
				virtual DatabaseReadonlyContactMessage getReadonlyMessage() const;
			protected:
				virtual QString getWhereString() const override;
				virtual void bindWhereStringValues(QSqlQuery& query) const override;
				virtual QString getTableName() const override;
			private:
				openmittsu::protocol::ContactId const m_contact;
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGECURSOR_H_
