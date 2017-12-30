#include "src/database/internal/DatabaseMessage.h"

#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/database/MediaFileItem.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseMessage::DatabaseMessage(InternalDatabaseInterface* database, openmittsu::protocol::MessageId const& messageId) : Message(), m_database(database), m_messageId(messageId) {
				//
			}

			DatabaseMessage::~DatabaseMessage() {
				//
			}

			openmittsu::protocol::MessageId const& DatabaseMessage::getMessageId() const {
				return m_messageId;
			}

			QVariant DatabaseMessage::queryField(QString const& fieldName) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `%1` FROM `%2` WHERE %3 AND `apiid` = :apiid;").arg(fieldName).arg(getTableName()).arg(getWhereString()));
				bindWhereStringValues(query);
				query.bindValue(QStringLiteral(":apiid"), QVariant(m_messageId.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute message field query for table " << getTableName().toStdString() << " with message ID \"" << m_messageId.toString() << "\" for field \"" << fieldName.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
				} else if (!query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "No message with message ID \"" << m_messageId.toString() << "\" exists, can not manipulate.";
				}

				return query.value(fieldName);
			}

			void DatabaseMessage::setFields(QVariantMap const& fieldsAndValues) {
				if (fieldsAndValues.size() > 0) {
					QSqlQuery query(m_database->getQueryObject());

					DatabaseUtilities::prepareSetFieldsUpdateQuery(query, QStringLiteral("UPDATE `%1` SET %3 WHERE %2 AND `apiid` = :apiid;").arg(getTableName()).arg(getWhereString()), fieldsAndValues);
					bindWhereStringValues(query);
					query.bindValue(QStringLiteral(":apiid"), QVariant(m_messageId.toQString()));

					if (!query.exec()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not update message data in " << getTableName().toStdString() << " for message ID \"" << m_messageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
					}

					announceMessageChanged();
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "DatabaseMessage::setFields() called with empty field/value map, this should never happen!";
				}
			}

			bool DatabaseMessage::isOutbox() const {
				return queryField(QStringLiteral("is_outbox")).toBool();
			}

			openmittsu::protocol::MessageTime DatabaseMessage::getCreatedAt() const {
				return openmittsu::protocol::MessageTime::fromDatabase(queryField(QStringLiteral("created_at")).toLongLong());
			}

			openmittsu::protocol::MessageTime DatabaseMessage::getSentAt() const {
				return openmittsu::protocol::MessageTime::fromDatabase(queryField(QStringLiteral("sent_at")).toLongLong());
			}

			openmittsu::protocol::MessageTime DatabaseMessage::getModifiedAt() const {
				return openmittsu::protocol::MessageTime::fromDatabase(queryField(QStringLiteral("modified_at")).toLongLong());
			}

			bool DatabaseMessage::isQueued() const {
				return queryField(QStringLiteral("is_queued")).toBool();
			}

			bool DatabaseMessage::isSent() const {
				return queryField(QStringLiteral("is_sent")).toBool();
			}

			QString DatabaseMessage::getUid() const {
				return queryField(QStringLiteral("uid")).toString();
			}

			QSqlQuery DatabaseMessage::getNewQuery() {
				return QSqlQuery(m_database->getQueryObject());
			}

			MediaFileItem DatabaseMessage::getMediaItem(QString const& uuid) const {
				return m_database->getMediaItem(uuid);
			}

			void DatabaseMessage::announceMessageChanged() {
				m_database->announceMessageChanged(getUid());
			}

			void DatabaseMessage::setIsSent() {
				QVariantMap fieldsAndValues;
				fieldsAndValues.insert(QStringLiteral("is_sent"), true);
				fieldsAndValues.insert(QStringLiteral("modified_at"), openmittsu::protocol::MessageTime::now().getMessageTimeMSecs());
				setFields(fieldsAndValues);
			}

			void DatabaseMessage::setIsQueued(bool isQueuedStatus) {
				QVariantMap fieldsAndValues;
				if (!isQueuedStatus) {
					fieldsAndValues.insert(QStringLiteral("is_sent"), false);
				}

				fieldsAndValues.insert(QStringLiteral("is_queued"), isQueuedStatus);
				fieldsAndValues.insert(QStringLiteral("modified_at"), openmittsu::protocol::MessageTime::now().getMessageTimeMSecs());
				setFields(fieldsAndValues);
			}

			openmittsu::protocol::ContactId DatabaseMessage::getSender() const {
				return openmittsu::protocol::ContactId(queryField(QStringLiteral("identity")).toString());
			}

			bool DatabaseMessage::isMessageFromUs() const {
				return queryField(QStringLiteral("is_outbox")).toInt() != 0;
			}

		}
	}
}
