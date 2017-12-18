#ifndef OPENMITTSU_DATABASE_DATABASECONTROLMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASECONTROLMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/database/DatabaseMessage.h"
#include "src/dataproviders/messages/ControlMessage.h"
#include "src/dataproviders/messages/ControlMessageState.h"
#include "src/dataproviders/messages/ControlMessageType.h"

namespace openmittsu {
	namespace database {
		class DatabaseControlMessage : public virtual DatabaseMessage, public virtual openmittsu::dataproviders::messages::ControlMessage {
		public:
			explicit DatabaseControlMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& controlMessageId, openmittsu::protocol::MessageId const& relatedMessageId, openmittsu::dataproviders::messages::ControlMessageType const& controlMessageType);
			virtual ~DatabaseControlMessage();

			virtual openmittsu::protocol::ContactId const& getContactId() const override;
			virtual openmittsu::dataproviders::messages::ControlMessageType getControlMessageType() const override;
			virtual openmittsu::dataproviders::messages::ControlMessageState getMessageState() const override;
			virtual void setMessageState(openmittsu::dataproviders::messages::ControlMessageState const& messageState, openmittsu::protocol::MessageTime const& when) override;

			static DatabaseControlMessage fromUuid(InternalDatabaseInterface* database, QString const& uuid);
			static DatabaseControlMessage fromReceiverAndControlMessageId(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& controlMessageId);

			static bool exists(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
			static bool hasControlMessageFor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& relatedMessageId, openmittsu::dataproviders::messages::ControlMessageType const& controlMessageType);
			static openmittsu::protocol::MessageId insertControlMessageFromUs(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& relatedMessageId, openmittsu::dataproviders::messages::ControlMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, bool isQueued, openmittsu::dataproviders::messages::ControlMessageType const& controlMessageType);
			static bool resetQueueStatus(InternalDatabaseInterface* database, int maxAgeInSeconds);
		protected:
			virtual QString getWhereString() const override;
			virtual void bindWhereStringValues(QSqlQuery& query) const override;
			virtual QString getTableName() const override;
		private:
			openmittsu::protocol::ContactId const m_contact;
			openmittsu::dataproviders::messages::ControlMessageType const m_controlMessageType;
			openmittsu::protocol::MessageId const m_relatedMessageId;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASECONTROLMESSAGE_H_
