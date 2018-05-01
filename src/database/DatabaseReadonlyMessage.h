#ifndef OPENMITTSU_DATABASE_DATABASEREADONLYMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEREADONLYMESSAGE_H_

#include <QString>
#include <QSqlQuery>
#include <QVariant>

#include "src/database/DatabaseWrapper.h"
#include "src/dataproviders/messages/ReadonlyMessage.h"

namespace openmittsu {
	namespace database {

		class DatabaseReadonlyMessage : public virtual openmittsu::dataproviders::messages::ReadonlyMessage {
		public:
			//explicit DatabaseReadonlyMessage(DatabaseWrapper const& database, QString const& uuid);
			DatabaseReadonlyMessage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid);
			virtual ~DatabaseReadonlyMessage();

			virtual openmittsu::protocol::ContactId getSender() const override;
			virtual openmittsu::protocol::MessageId const& getMessageId() const override;

			virtual bool isMessageFromUs() const override;

			virtual openmittsu::protocol::MessageTime getCreatedAt() const override;
			virtual openmittsu::protocol::MessageTime getSentAt() const override;
			virtual openmittsu::protocol::MessageTime getModifiedAt() const override;

			/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
			virtual bool isQueued() const override;

			/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
			virtual bool isSent() const override;

			virtual QString getUid() const override;
		protected:
			openmittsu::protocol::ContactId m_sender;
			openmittsu::protocol::MessageId m_messageId;
			bool m_isMessageFromUs;
			openmittsu::protocol::MessageTime m_createdAt;
			openmittsu::protocol::MessageTime m_sentAt;
			openmittsu::protocol::MessageTime m_modifiedAt;
			bool m_isQueued;
			bool m_isSent;
			QString m_uuid;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEREADONLYMESSAGE_H_
