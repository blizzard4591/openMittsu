#ifndef OPENMITTSU_DATABASE_INTERNAL_DATABASEGROUPMESSAGECURSOR_H_
#define OPENMITTSU_DATABASE_INTERNAL_DATABASEGROUPMESSAGECURSOR_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/DatabaseReadonlyGroupMessage.h"
#include "src/database/internal/DatabaseGroupMessage.h"
#include "src/database/internal/DatabaseMessageCursor.h"
#include "src/dataproviders/messages/GroupMessageCursor.h"

namespace openmittsu {
	namespace database {
		namespace internal {

			class DatabaseGroupMessageCursor : public virtual DatabaseMessageCursor, public virtual openmittsu::dataproviders::messages::GroupMessageCursor {
			public:
				explicit DatabaseGroupMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group);
				explicit DatabaseGroupMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId);
				explicit DatabaseGroupMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, QString const& uuid);
				virtual ~DatabaseGroupMessageCursor();

				virtual openmittsu::protocol::GroupId const& getGroupId() const override;
				virtual std::shared_ptr<openmittsu::dataproviders::messages::GroupMessage> getMessage() const override;
				virtual std::shared_ptr<DatabaseReadonlyGroupMessage> getReadonlyMessage() const;

				static void deleteMessagesByAge(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint);
				static void deleteMessagesByCount(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, bool oldestOrNewest, int count);
			protected:
				virtual QString getWhereString() const override;
				virtual void bindWhereStringValues(QSqlQuery& query) const override;
				virtual QString getTableName() const override;
				virtual QString getMessageTypeField() const override;
			private:
				openmittsu::protocol::GroupId const m_group;

				static void deletionHelper(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, QString const& whereAndOrderQueryPart);
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASEGROUPMESSAGECURSOR_H_
