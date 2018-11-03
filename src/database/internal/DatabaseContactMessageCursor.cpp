#include "src/database/internal/DatabaseContactMessageCursor.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>


namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseContactMessageCursor::DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact) : DatabaseMessageCursor(database), ContactMessageCursor(), m_contact(contact) {
				//
			}

			DatabaseContactMessageCursor::~DatabaseContactMessageCursor() {
				//
			}

			DatabaseContactMessageCursor::DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) : DatabaseMessageCursor(database), ContactMessageCursor(), m_contact(contact) {
				if (!seek(messageId)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, invalid entry point.";
				}
			}

			DatabaseContactMessageCursor::DatabaseContactMessageCursor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, QString const& uuid) : DatabaseMessageCursor(database), ContactMessageCursor(), m_contact(contact) {
				if (!seekByUuid(uuid)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and UUID \"" << uuid.toStdString() << "\" exists, invalid entry point.";
				}
			}

			openmittsu::protocol::ContactId const& DatabaseContactMessageCursor::getContactId() const {
				return m_contact;
			}

			std::shared_ptr<ContactMessage> DatabaseContactMessageCursor::getMessage() const {
				if (!isValid()) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not create message wrapper for invalid message.";
				}

				return std::make_shared<DatabaseContactMessage>(getDatabase(), m_contact, getMessageId());
			}

			std::shared_ptr<DatabaseReadonlyContactMessage> DatabaseContactMessageCursor::getReadonlyMessage() const {
				if (!isValid()) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not create message wrapper for invalid message.";
				}

				QSqlQuery query(getDatabase()->getQueryObject());
				// openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, 
				// openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, 
				// openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption, openmittsu::protocol::ContactId const& contact, 
				// openmittsu::dataproviders::messages::ContactMessageType const& contactMessageType, QString const& body, MediaFileItem const& mediaItem
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `uid`, `is_outbox`, `is_read`, `is_saved`, `messagestate`, `sort_by`, `created_at`, `sent_at`, `received_at`, `seen_at`, `modified_at`, `contact_message_type`, `body`, `is_statusmessage`, `is_queued`, `is_sent`, `caption` FROM `contact_messages` WHERE `identity` = :identity AND `uid` = :uid;"));
				bindWhereStringValues(query);
				query.bindValue(QStringLiteral(":uid"), QVariant(getMessageUuid()));
				if (!query.exec() || !query.isSelect() || !query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact message query for table contact_messages. Query error: " << query.lastError().text().toStdString();
				}

				openmittsu::protocol::ContactId const contact(query.value(QStringLiteral("identity")).toString());
				openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
				bool const isMessageFromUs = query.value(QStringLiteral("is_outbox")).toBool();
				openmittsu::protocol::MessageTime const createdAt(openmittsu::protocol::MessageTime::fromDatabase(query.value(QStringLiteral("created_at")).toLongLong()));
				openmittsu::protocol::MessageTime const sentAt(openmittsu::protocol::MessageTime::fromDatabase(query.value(QStringLiteral("sent_at")).toLongLong()));
				openmittsu::protocol::MessageTime const modifiedAt(openmittsu::protocol::MessageTime::fromDatabase(query.value(QStringLiteral("modified_at")).toLongLong()));
				bool const isQueued = query.value(QStringLiteral("is_queued")).toBool();
				bool const isSent = query.value(QStringLiteral("is_sent")).toBool();
				QString const uuid(query.value(QStringLiteral("uid")).toString());
				bool const isRead = query.value(QStringLiteral("is_read")).toBool();
				bool const isSaved = query.value(QStringLiteral("is_saved")).toBool();
				UserMessageState const messageState(UserMessageStateHelper::fromString(query.value(QStringLiteral("messagestate")).toString()));
				openmittsu::protocol::MessageTime const receivedAt(openmittsu::protocol::MessageTime::fromDatabase(query.value(QStringLiteral("received_at")).toLongLong()));
				openmittsu::protocol::MessageTime const seenAt(openmittsu::protocol::MessageTime::fromDatabase(query.value(QStringLiteral("seen_at")).toLongLong()));
				bool const isStatusMessage = query.value(QStringLiteral("is_statusmessage")).toBool();
				QString const caption(query.value(QStringLiteral("caption")).toString());
				openmittsu::dataproviders::messages::ContactMessageType const contactMessageType(openmittsu::dataproviders::messages::ContactMessageTypeHelper::fromString(query.value(QStringLiteral("contact_message_type")).toString()));
				QString const body(query.value(QStringLiteral("body")).toString());

				openmittsu::database::MediaFileItem mediaItem;
				if ((contactMessageType == openmittsu::dataproviders::messages::ContactMessageType::AUDIO) || (contactMessageType == openmittsu::dataproviders::messages::ContactMessageType::FILE) || (contactMessageType == openmittsu::dataproviders::messages::ContactMessageType::IMAGE) || (contactMessageType == openmittsu::dataproviders::messages::ContactMessageType::VIDEO)) {
					mediaItem = getDatabase()->getMediaItem(uuid, MediaFileType::TYPE_STANDARD);
				} else {
					mediaItem = MediaFileItem(MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE, MediaFileType::TYPE_STANDARD);
				}

				auto drcm = std::make_shared<DatabaseReadonlyContactMessage>(contact, messageId, isMessageFromUs, createdAt, sentAt, modifiedAt, isQueued, isSent, uuid, isRead, isSaved, messageState, receivedAt, seenAt, isStatusMessage, caption, contactMessageType, body, mediaItem);
				if (!drcm) {
					throw openmittsu::exceptions::InternalErrorException() << "Fetching a group message to readonly failed for group " << contact.toString() << " and UUID " << getMessageUuid().toStdString() << "!";
				}
				return drcm;
			}

			QString DatabaseContactMessageCursor::getWhereString() const {
				return QStringLiteral("`identity` = :identity");
			}

			void DatabaseContactMessageCursor::bindWhereStringValues(QSqlQuery& query) const {
				query.bindValue(QStringLiteral(":identity"), QVariant(m_contact.toQString()));
			}

			QString DatabaseContactMessageCursor::getTableName() const {
				return QStringLiteral("contact_messages");
			}

			QString DatabaseContactMessageCursor::getMessageTypeField() const {
				return QStringLiteral("contact_message_type");
			}

			void DatabaseContactMessageCursor::deleteMessagesByAge(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) {
				QString whereAndOrderPart(QStringLiteral("AND `sort_by` %1 %2 ORDER BY `sort_by` %3"));
				if (olderThanOrNewerThan) {
					whereAndOrderPart = whereAndOrderPart.arg(QStringLiteral("<=")).arg(timePoint.getMessageTimeMSecs()).arg(QStringLiteral("ASC"));
				} else {
					whereAndOrderPart = whereAndOrderPart.arg(QStringLiteral(">=")).arg(timePoint.getMessageTimeMSecs()).arg(QStringLiteral("DESC"));
				}

				deletionHelper(database, contact, whereAndOrderPart);
			}
			
			void DatabaseContactMessageCursor::deleteMessagesByCount(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, bool oldestOrNewest, int count) {
				QString whereAndOrderPart(QStringLiteral("ORDER BY `sort_by` %1 LIMIT %2"));
				if (oldestOrNewest) {
					whereAndOrderPart = whereAndOrderPart.arg(QStringLiteral("ASC")).arg(count);
				} else {
					whereAndOrderPart = whereAndOrderPart.arg(QStringLiteral("DESC")).arg(count);
				}

				deletionHelper(database, contact, whereAndOrderPart);
			}

			void DatabaseContactMessageCursor::deletionHelper(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, QString const& whereAndOrderQueryPart) {
				database->transactionStart();
				QVector<QString> uuids;
				QString const selectQuery = QStringLiteral("SELECT `uid` FROM `contact_messages` WHERE `identity` = :identity %1").arg(whereAndOrderQueryPart);
				{
					QSqlQuery query(database->getQueryObject());
					if (!query.prepare(selectQuery)) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not prepare contact message enumeration query. SQL error: " << query.lastError().text().toStdString();
					}
					query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
					if (!query.exec() || !query.isSelect()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact message enumeration query for table contact_messages. Query error: " << query.lastError().text().toStdString();
					}

					while (query.next()) {
						QString const uuid(query.value(QStringLiteral("uid")).toString());
						uuids.append(uuid);
					}
				}
				{
					QSqlQuery query(database->getQueryObject());
					QString const deleteQuery = QStringLiteral("DELETE FROM `contact_messages` WHERE `uid` in (%1)").arg(selectQuery);
					if (!query.prepare(deleteQuery)) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not prepare contact message mass deletion query. SQL error: " << query.lastError().text().toStdString();
					}
					query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
					if (!query.exec()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact message mass deletion query for table contact_messages. Query error: " << query.lastError().text().toStdString();
					}
					
				}
				{
					auto it = uuids.constBegin();
					auto const end = uuids.constEnd();
					for (; it != end; ++it) {
						database->removeAllMediaItems(*it);
					}
				}
				database->transactionCommit();

				auto it = uuids.constBegin();
				auto const end = uuids.constEnd();
				for (; it != end; ++it) {
					database->announceMessageDeleted(*it);
				}
			}
		}
	}
}
