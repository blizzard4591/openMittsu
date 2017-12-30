#ifndef OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGE_H_
#define OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGE_H_

#include <QList>
#include <QString>

#include "src/protocol/ContactId.h"
#include "src/database/internal/DatabaseUserMessage.h"
#include "src/dataproviders/messages/ContactMessage.h"
#include "src/dataproviders/messages/ContactMessageType.h"

namespace openmittsu {
	namespace backup {
		class ContactMessageBackupObject;
	}

	namespace database {
		namespace internal {
			class DatabaseContactMessage : public virtual DatabaseUserMessage, public virtual openmittsu::dataproviders::messages::ContactMessage {
			public:
				explicit DatabaseContactMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
				virtual ~DatabaseContactMessage();

				virtual openmittsu::protocol::ContactId const& getContactId() const override;
				virtual openmittsu::dataproviders::messages::ContactMessageType getMessageType() const override;

				virtual QString getContentAsText() const override;
				virtual openmittsu::utility::Location getContentAsLocation() const override;
				virtual MediaFileItem getContentAsImage() const override;

				static int getContactMessageCount(InternalDatabaseInterface* database);
				static int getContactMessageCount(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact);

				static bool exists(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
				static openmittsu::protocol::MessageId insertContactMessageFromUs(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, QString const& uuid, openmittsu::protocol::MessageTime const& createdAt, openmittsu::dataproviders::messages::ContactMessageType const& type, QString const& body, bool isQueued, bool isStatusMessage, QString const& caption);
				static void insertContactMessageFromThem(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId, QString const& uuid, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::dataproviders::messages::ContactMessageType const& type, QString const& body, bool isStatusMessage, QString const& caption);
				static void insertContactMessagesFromBackup(InternalDatabaseInterface* database, QList<openmittsu::backup::ContactMessageBackupObject> const& messages);
				static bool resetQueueStatus(InternalDatabaseInterface* database, int maxAgeInSeconds);
			protected:
				virtual QString getWhereString() const override;
				virtual void bindWhereStringValues(QSqlQuery& query) const override;
				virtual QString getTableName() const override;
			private:
				openmittsu::protocol::ContactId const m_contact;

				static void insertContactMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool isOutgoing, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, openmittsu::protocol::MessageTime const& modifiedAt, openmittsu::dataproviders::messages::ContactMessageType const& type, QString const& body, bool isStatusMessage, bool isQueued, bool isSent, QString const& caption);
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASECONTACTMESSAGE_H_
