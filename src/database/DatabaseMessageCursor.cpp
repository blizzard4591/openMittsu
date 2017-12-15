#include "src/database/DatabaseMessageCursor.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseMessageCursor::DatabaseMessageCursor(SimpleDatabase& database) : m_database(database), m_messageId(0), m_isMessageIdValid(false) {
			//
		}
		
		DatabaseMessageCursor::~DatabaseMessageCursor() {
			//
		}

		bool DatabaseMessageCursor::isValid() const {
			return m_isMessageIdValid;
		}

		bool DatabaseMessageCursor::seekToFirst() {
			return getFirstOrLastMessageId(true);
		}

		bool DatabaseMessageCursor::seekToLast() {
			return getFirstOrLastMessageId(false);
		}

		bool DatabaseMessageCursor::seek(openmittsu::protocol::MessageId const& messageId) {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE %2 AND `apiid` = :apiid;").arg(getTableName()).arg(getWhereString()));
			query.bindValue(QStringLiteral(":apiid"), QVariant(messageId.toQString()));
			bindWhereStringValues(query);

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message seek query for table " << getTableName().toStdString() << " for message ID \"" << messageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				m_isMessageIdValid = true;
				m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
				m_uid = query.value(QStringLiteral("uid")).toString();
				m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
				return true;
			} else {
				m_isMessageIdValid = false;
				return false;
			}
		}

		bool DatabaseMessageCursor::seekByUuid(QString const& uuid) {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE %2 AND `uid` = :uid;").arg(getTableName()).arg(getWhereString()));
			query.bindValue(QStringLiteral(":uid"), QVariant(uuid));
			bindWhereStringValues(query);

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message seek query for table " << getTableName().toStdString() << " for UUID \"" << uuid.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				m_isMessageIdValid = true;
				m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
				m_uid = query.value(QStringLiteral("uid")).toString();
				m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
				return true;
			} else {
				m_isMessageIdValid = false;
				return false;
			}
		}

		bool DatabaseMessageCursor::getFollowingMessageId(bool ascending) {
			if (!m_isMessageIdValid) {
				return false;
			}

			QString sortOrder;
			QString sortOrderSign;
			if (ascending) {
				sortOrder = QStringLiteral("ASC");
				sortOrderSign = QStringLiteral(">");
			} else {
				sortOrder = QStringLiteral("DESC");
				sortOrderSign = QStringLiteral("<");
			}

#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
			// Check in two steps to mitigate a cool bug in the query engine.
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE (%2) AND (((`sort_by` = :sortByValue) AND (`uid` %3 :uid))) ORDER BY `sort_by` %4, `uid` %4 LIMIT 1;").arg(getTableName()).arg(getWhereString()).arg(sortOrderSign).arg(sortOrder));
			bindWhereStringValues(query);
			query.bindValue(QStringLiteral(":sortByValue"), QVariant(m_sortByValue));
			query.bindValue(QStringLiteral(":uid"), QVariant(m_uid));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message iteration query for table " << getTableName().toStdString() << ". Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				m_isMessageIdValid = true;
				m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
				m_uid = query.value(QStringLiteral("uid")).toString();
				m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
				return true;
			} else {
				query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE (%2) AND ((`sort_by` %3 :sortByValue)) ORDER BY `sort_by` %4, `uid` %4 LIMIT 1;").arg(getTableName()).arg(getWhereString()).arg(sortOrderSign).arg(sortOrder));
				bindWhereStringValues(query);
				query.bindValue(QStringLiteral(":sortByValue"), QVariant(m_sortByValue));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute message iteration query for table " << getTableName().toStdString() << ". Query error: " << query.lastError().text().toStdString();
				}

				if (query.next()) {
					m_isMessageIdValid = true;
					m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
					m_uid = query.value(QStringLiteral("uid")).toString();
					m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
					return true;
				} else {
					return false;
				}
			}
#else
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE %2 AND ((`sort_by` %3 :sortByValue) OR ((`sort_by` = :sortByValue) AND (`uid` %3 :uid))) ORDER BY `sort_by` %4, `uid` %4 LIMIT 1;").arg(getTableName()).arg(getWhereString()).arg(sortOrderSign).arg(sortOrder));
			bindWhereStringValues(query);
			query.bindValue(QStringLiteral(":sortByValue"), QVariant(m_sortByValue));
			query.bindValue(QStringLiteral(":uid"), QVariant(m_uid));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message iteration query for table " << getTableName().toStdString() << ". Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				m_isMessageIdValid = true;
				m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
				m_uid = query.value(QStringLiteral("uid")).toString();
				m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
				return true;
			} else {
				return false;
			}
#endif
		}

		bool DatabaseMessageCursor::getFirstOrLastMessageId(bool first) {
			QString sortOrder;
			if (first) {
				sortOrder = QStringLiteral("ASC");
			} else {
				sortOrder = QStringLiteral("DESC");
			}

			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `apiid`, `uid`, `sort_by` FROM `%1` WHERE %2 ORDER BY `sort_by` %3, `uid` %3 LIMIT 1;").arg(getTableName()).arg(getWhereString()).arg(sortOrder));
			bindWhereStringValues(query);

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message first/last query for table " << getTableName().toStdString() << ". Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				m_isMessageIdValid = true;
				m_messageId = openmittsu::protocol::MessageId(query.value(QStringLiteral("apiid")).toString());
				m_uid = query.value(QStringLiteral("uid")).toString();
				m_sortByValue = query.value(QStringLiteral("sort_by")).toLongLong();
				return true;
			} else {
				return false;
			}
		}

		QVector<QString> DatabaseMessageCursor::getLastMessages(std::size_t n) const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `uid` FROM `%1` WHERE %2 ORDER BY `sort_by` DESC, `uid` DESC LIMIT %3;").arg(getTableName()).arg(getWhereString()).arg(n));
			bindWhereStringValues(query);

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message enumeration query for table " << getTableName().toStdString() << ". Query error: " << query.lastError().text().toStdString();
			}

			QVector<QString> result;

			while (query.next()) {
				result.push_back(query.value(QStringLiteral("uid")).toString());
			}

			return result;
		}

		bool DatabaseMessageCursor::next() {
			return getFollowingMessageId(true);
		}

		bool DatabaseMessageCursor::previous() {
			return getFollowingMessageId(false);
		}

		SimpleDatabase& DatabaseMessageCursor::getDatabase() const {
			return m_database;
		}

		openmittsu::protocol::MessageId const& DatabaseMessageCursor::getMessageId() const {
			return m_messageId;
		}

	}
}
