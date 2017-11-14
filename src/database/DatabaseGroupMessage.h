#ifndef OPENMITTSU_DATABASE_DATABASEGROUPMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEGROUPMESSAGE_H_

#include <QList>
#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/database/DatabaseUserMessage.h"
#include "src/dataproviders/messages/GroupMessage.h"
#include "src/dataproviders/messages/GroupMessageType.h"

namespace openmittsu {
	namespace backup {
		class GroupMessageBackupObject;
	}

	namespace database {

		class Database;

		class DatabaseGroupMessage : public virtual DatabaseUserMessage, public virtual openmittsu::dataproviders::messages::GroupMessage {
		public:
			DatabaseGroupMessage(Database& database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId);
			virtual ~DatabaseGroupMessage();

			virtual openmittsu::protocol::GroupId const& getGroupId() const override;
			virtual openmittsu::dataproviders::messages::GroupMessageType getMessageType() const override;

			virtual QString getContentAsText() const override;
			virtual openmittsu::utility::Location getContentAsLocation() const override;
			virtual QByteArray getContentAsImage() const override;

			static int getGroupMessageCount(Database const& database);
			static int getGroupMessageCount(Database const& database, openmittsu::protocol::GroupId const& group);

			static bool exists(Database& database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId);
			static openmittsu::protocol::MessageId insertGroupMessageFromUs(Database& database, openmittsu::protocol::GroupId const& group, QString const& uuid, openmittsu::protocol::MessageTime const& createdAt, openmittsu::dataproviders::messages::GroupMessageType const& type, QString const& body, bool isQueued, bool isStatusMessage, QString const& caption);
			static void insertGroupMessageFromThem(Database& database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, QString const& uuid, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::dataproviders::messages::GroupMessageType const& type, QString const& body, bool isStatusMessage, QString const& caption);
			static void insertGroupMessagesFromBackup(Database& database, QList<openmittsu::backup::GroupMessageBackupObject> const& messages);
			static bool resetQueueStatus(Database& database, int maxAgeInSeconds);
		protected:
			virtual QString getWhereString() const override;
			virtual void bindWhereStringValues(QSqlQuery& query) const override;
			virtual QString getTableName() const override;
		private:
			openmittsu::protocol::GroupId const m_group;

			static void insertGroupMessage(Database& database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool isOutgoing, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, openmittsu::protocol::MessageTime const& modifiedAt, openmittsu::dataproviders::messages::GroupMessageType const& type, QString const& body, bool isStatusMessage, bool isQueued, bool isSent, QString const& caption);
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEGROUPMESSAGE_H_
