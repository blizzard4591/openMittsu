#ifndef OPENMITTSU_DATABASE_DATABASEMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEMESSAGE_H_

#include <QString>
#include <QSqlQuery>
#include <QVariant>

#include "src/dataproviders/messages/Message.h"

namespace openmittsu {
	namespace database {

		class Database;

		class DatabaseMessage : public virtual openmittsu::dataproviders::messages::Message {
		public:
			explicit DatabaseMessage(Database& database, openmittsu::protocol::MessageId const& messageId);
			virtual ~DatabaseMessage();

			virtual openmittsu::protocol::ContactId getSender() const override;
			virtual openmittsu::protocol::MessageId const& getMessageId() const override;

			virtual bool isMessageFromUs() const override;

			/** Encodes whether this is an outgoing or incoming message. */
			virtual bool isOutbox() const override;

			virtual openmittsu::protocol::MessageTime getCreatedAt() const override;
			virtual openmittsu::protocol::MessageTime getSentAt() const override;
			virtual openmittsu::protocol::MessageTime getModifiedAt() const override;

			/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
			virtual bool isQueued() const override;

			/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
			virtual bool isSent() const override;

			virtual void setIsQueued(bool isQueuedStatus) override;
			virtual void setIsSent() override;

			virtual QString getUid() const override;
		protected:
			QSqlQuery getNewQuery();

			virtual QString getWhereString() const = 0;
			virtual void bindWhereStringValues(QSqlQuery& query) const = 0;
			virtual QString getTableName() const = 0;

			QVariant queryField(QString const& fieldName) const;
			void setFields(QVariantMap const& fieldsAndValues);

			QByteArray getMediaItem(QString const& uuid) const;

			void announceMessageChanged();
		private:
			Database& m_database;
			openmittsu::protocol::MessageId const m_messageId;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASECONTACTMESSAGE_H_
