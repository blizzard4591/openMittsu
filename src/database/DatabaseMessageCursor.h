#ifndef OPENMITTSU_DATABASE_DATABASEMESSAGECURSOR_H_
#define OPENMITTSU_DATABASE_DATABASEMESSAGECURSOR_H_

#include <QString>
#include <QSqlQuery>
#include <QVector>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/DatabaseContactMessage.h"
#include "src/dataproviders/messages/MessageCursor.h"

namespace openmittsu {
	namespace database {
		class Database;

		class DatabaseMessageCursor : public virtual openmittsu::dataproviders::messages::MessageCursor {
		public:
			explicit DatabaseMessageCursor(Database& database);
			virtual ~DatabaseMessageCursor();

			virtual bool isValid() const override;
			virtual bool seekToFirst() override;
			virtual bool seekToLast() override;
			virtual bool seek(openmittsu::protocol::MessageId const& messageId) override;
			virtual bool seekByUuid(QString const& uuid) override;

			virtual bool next() override;
			virtual bool previous() override;

			virtual openmittsu::protocol::MessageId const& getMessageId() const override;
			virtual QVector<QString> getLastMessages(std::size_t n) const override;
		protected:
			Database& getDatabase() const;

			virtual QString getWhereString() const = 0;
			virtual void bindWhereStringValues(QSqlQuery& query) const = 0;
			virtual QString getTableName() const = 0;
		private:
			Database& m_database;
			openmittsu::protocol::MessageId m_messageId;
			bool m_isMessageIdValid;
			qint64 m_sortByValue;
			QString m_uid;

			bool getFollowingMessageId(bool ascending);
			bool getFirstOrLastMessageId(bool first);
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEMESSAGECURSOR_H_
