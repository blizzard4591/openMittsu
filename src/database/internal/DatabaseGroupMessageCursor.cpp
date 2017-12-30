#include "src/database/internal/DatabaseGroupMessageCursor.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseGroupMessageCursor::DatabaseGroupMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group) : DatabaseMessageCursor(database), GroupMessageCursor(), m_group(group) {
				//
			}

			DatabaseGroupMessageCursor::DatabaseGroupMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) : DatabaseMessageCursor(database), m_group(group) {
				if (!seek(messageId)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from group \"" << group.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, invalid entry point.";
				}
			}

			DatabaseGroupMessageCursor::~DatabaseGroupMessageCursor() {
				//
			}

			openmittsu::protocol::GroupId const& DatabaseGroupMessageCursor::getGroupId() const {
				return m_group;
			}

			std::shared_ptr<GroupMessage> DatabaseGroupMessageCursor::getMessage() const {
				if (!isValid()) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not create message wrapper for invalid message.";
				}

				return std::make_shared<DatabaseGroupMessage>(getDatabase(), m_group, getMessageId());
			}

			QString DatabaseGroupMessageCursor::getWhereString() const {
				return QStringLiteral("`group_id` = :groupId AND `group_creator` = :groupCreator");
			}

			void DatabaseGroupMessageCursor::bindWhereStringValues(QSqlQuery& query) const {
				query.bindValue(QStringLiteral(":groupId"), QVariant(m_group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(m_group.getOwner().toQString()));
			}

			QString DatabaseGroupMessageCursor::getTableName() const {
				return QStringLiteral("group_messages");
			}

		}
	}
}
