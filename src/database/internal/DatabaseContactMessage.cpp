#include "src/database/internal/DatabaseContactMessage.h"

#include "src/backup/ContactMessageBackupObject.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>
#include <chrono>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseContactMessage::DatabaseContactMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) : DatabaseMessage(database, messageId), DatabaseUserMessage(database, messageId), ContactMessage(), m_contact(contact) {
				if (!exists(database, contact, messageId)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from contact \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, can not manipulate.";
				}
			}

			DatabaseContactMessage::~DatabaseContactMessage() {
				//
			}

			openmittsu::protocol::ContactId const& DatabaseContactMessage::getContactId() const {
				return m_contact;
			}

			int DatabaseContactMessage::getContactMessageCount(InternalDatabaseInterface* database) {
				return openmittsu::database::internal::DatabaseUtilities::countQuery(database, QStringLiteral("contact_messages"));
			}

			int DatabaseContactMessage::getContactMessageCount(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact) {
				return openmittsu::database::internal::DatabaseUtilities::countQuery(database, QStringLiteral("contact_messages"), { { QStringLiteral("identity"), contact.toQString() } });
			}

			bool DatabaseContactMessage::exists(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `apiid` FROM `contact_messages` WHERE `identity` = :identity AND `apiid` = :apiid;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(messageId.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact message existance query for table contact_messages for identity \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}
				return query.next();
			}

			ContactMessageType DatabaseContactMessage::getMessageType() const {
				return ContactMessageTypeHelper::fromString(queryField(QStringLiteral("contact_message_type")).toString());
			}

			QString DatabaseContactMessage::getWhereString() const {
				return QStringLiteral("`identity` = :identity");
			}

			void DatabaseContactMessage::bindWhereStringValues(QSqlQuery& query) const {
				query.bindValue(QStringLiteral(":identity"), QVariant(m_contact.toQString()));
			}

			QString DatabaseContactMessage::getTableName() const {
				return QStringLiteral("contact_messages");
			}

			void DatabaseContactMessage::insertContactMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool isOutgoing, bool isRead, bool isSaved, UserMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, openmittsu::protocol::MessageTime const& modifiedAt, ContactMessageType const& type, QString const& body, bool isStatusMessage, bool isQueued, bool isSent, QString const& caption) {
				QSqlQuery query(database->getQueryObject());

				query.prepare(QStringLiteral("INSERT INTO `contact_messages` (`identity`, `apiid`, `uid`, `is_outbox`, `is_read`, `is_saved`, `messagestate`, `sort_by`, `created_at`, `sent_at`, `received_at`, `seen_at`, `modified_at`, `contact_message_type`, `body`, `is_statusmessage`, `is_queued`, `is_sent`, `caption`) VALUES "
											 "(:identity, :apiid, :uid, :isOutbox, :isRead, :isSaved, :messageState, :sortBy, :createdAt, :sentAt, :receivedAt, :seenAt, :modifiedAt, :type, :body, :isStatusMessage, :isQueued, :isSent, :caption);"));
				query.bindValue(QStringLiteral(":identity"), QVariant(receiver.toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(apiId.toQString()));
				query.bindValue(QStringLiteral(":uid"), QVariant(uuid));
				query.bindValue(QStringLiteral(":isOutbox"), QVariant(isOutgoing));
				query.bindValue(QStringLiteral(":isRead"), QVariant(isRead));
				query.bindValue(QStringLiteral(":isSaved"), QVariant(isSaved));
				query.bindValue(QStringLiteral(":messageState"), QVariant(UserMessageStateHelper::toString(messageState)));
				qint64 const sortByValue = ((isOutgoing) ? (createdAt.getMessageTimeMSecs()) : (receivedAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":sortBy"), QVariant(sortByValue));
				query.bindValue(QStringLiteral(":createdAt"), (createdAt.isNull()) ? QVariant() : QVariant(createdAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":sentAt"), (sentAt.isNull()) ? QVariant() : QVariant(sentAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":receivedAt"), (receivedAt.isNull()) ? QVariant() : QVariant(receivedAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":seenAt"), (seenAt.isNull()) ? QVariant() : QVariant(seenAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":modifiedAt"), (modifiedAt.isNull()) ? QVariant() : QVariant(modifiedAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":type"), QVariant(ContactMessageTypeHelper::toQString(type)));
				query.bindValue(QStringLiteral(":body"), QVariant(body));
				query.bindValue(QStringLiteral(":isStatusMessage"), QVariant(isStatusMessage));
				query.bindValue(QStringLiteral(":isQueued"), QVariant(isQueued));
				query.bindValue(QStringLiteral(":isSent"), QVariant(isSent));
				query.bindValue(QStringLiteral(":caption"), QVariant(caption));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert contact message data into 'contact_messages'. Query error: " << query.lastError().text().toStdString();
				}
			}

			bool DatabaseContactMessage::resetQueueStatus(InternalDatabaseInterface* database, int maxAgeInSeconds) {
				QSqlQuery query(database->getQueryObject());

				openmittsu::protocol::MessageTime const limit(QDateTime::currentDateTime().addSecs(-maxAgeInSeconds));
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `uid` FROM `contact_messages` WHERE `is_outbox` = 1 AND `is_queued` = 1 AND `is_sent` = 0 AND ((`modified_at` <= %1) OR (`modified_at` IS NULL));").arg(limit.getMessageTimeMSecs()));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact message queue reset query for table contact_messages. Query error: " << query.lastError().text().toStdString();
				}

				bool hasAtLeastOneEntry = false;
				while (query.next()) {
					hasAtLeastOneEntry = true;

					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					DatabaseContactMessage message(database, receiver, messageId);
					message.setIsQueued(false);
				}

				return hasAtLeastOneEntry;
			}

			openmittsu::protocol::MessageId DatabaseContactMessage::insertContactMessageFromUs(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, QString const& uuid, openmittsu::protocol::MessageTime const& createdAt, ContactMessageType const& type, QString const& body, bool isQueued, bool isStatusMessage, QString const& caption) {
				openmittsu::protocol::MessageId const messageId = database->getNextMessageId(contact);

				insertContactMessage(database, contact, messageId, uuid, true, false, true, UserMessageState::SENDING, createdAt, openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), type, body, isStatusMessage, isQueued, false, caption);

				database->announceNewMessage(contact, uuid);

				return messageId;
			}

			void DatabaseContactMessage::insertContactMessageFromThem(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId, QString const& uuid, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, ContactMessageType const& type, QString const& body, bool isStatusMessage, QString const& caption) {
				insertContactMessage(database, contact, messageId, uuid, false, false, true, UserMessageState::DELIVERED, sentAt, sentAt, receivedAt, openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), type, body, isStatusMessage, true, true, caption);

				database->announceNewMessage(contact, uuid);
				database->announceReceivedNewMessage(contact);
			}

			void DatabaseContactMessage::insertContactMessagesFromBackup(InternalDatabaseInterface* database, QList<openmittsu::backup::ContactMessageBackupObject> const& messages) {
				auto startTime = std::chrono::high_resolution_clock::now();
				QVariantList identity;
				identity.reserve(messages.size());
				QVariantList apiid;
				apiid.reserve(messages.size());
				QVariantList uid;
				uid.reserve(messages.size());
				QVariantList isOutbox;
				isOutbox.reserve(messages.size());
				QVariantList isRead;
				isRead.reserve(messages.size());
				QVariantList isSaved;
				isSaved.reserve(messages.size());
				QVariantList messageState;
				messageState.reserve(messages.size());
				QVariantList sortBy;
				sortBy.reserve(messages.size());
				QVariantList createdAt;
				createdAt.reserve(messages.size());
				QVariantList sentAt;
				sentAt.reserve(messages.size());
				QVariantList receivedAt;
				receivedAt.reserve(messages.size());
				QVariantList seenAt;
				seenAt.reserve(messages.size());
				QVariantList modifiedAt;
				modifiedAt.reserve(messages.size());
				QVariantList type;
				type.reserve(messages.size());
				QVariantList body;
				body.reserve(messages.size());
				QVariantList isStatusMessage;
				isStatusMessage.reserve(messages.size());
				QVariantList isQueued;
				isQueued.reserve(messages.size());
				QVariantList isSent;
				isSent.reserve(messages.size());
				QVariantList caption;
				caption.reserve(messages.size());

				auto it = messages.constBegin();
				auto end = messages.constEnd();
				for (; it != end; ++it) {
					identity.append(it->getContactId().toQString());
					apiid.append(it->getApiId().toQString());
					uid.append(it->getUuid());
					isOutbox.append(it->getIsOutbox());
					isRead.append(it->getIsRead());
					isSaved.append(it->getIsSaved());
					messageState.append(UserMessageStateHelper::toString(it->getMessageState()));
					sortBy.append(((it->getIsOutbox()) ? (it->getCreatedAt().getMessageTimeMSecs()) : (it->getReceivedAt().getMessageTimeMSecs())));
					createdAt.append((it->getCreatedAt().isNull()) ? QVariant() : QVariant(it->getCreatedAt().getMessageTimeMSecs()));
					sentAt.append((it->getSentAt().isNull()) ? QVariant() : QVariant(it->getSentAt().getMessageTimeMSecs()));
					receivedAt.append((it->getReceivedAt().isNull()) ? QVariant() : QVariant(it->getReceivedAt().getMessageTimeMSecs()));
					seenAt.append((!it->getIsRead()) ? QVariant() : QVariant(it->getModifiedAt().getMessageTimeMSecs()));
					modifiedAt.append((it->getModifiedAt().isNull()) ? QVariant() : QVariant(it->getModifiedAt().getMessageTimeMSecs()));
					type.append(ContactMessageTypeHelper::toQString(it->getMessageType()));
					body.append(it->getBody());
					isStatusMessage.append(it->getIsStatusMessage());
					isQueued.append(it->getIsQueued());
					isSent.append(UserMessageStateHelper::isSent(it->getMessageState()));
					caption.append(it->getCaption());
				}

				//if (!database.database.transaction()) {
				if (!database->transactionStart()) {
					LOGGER()->warn("Could NOT start transaction!");
				}

				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("INSERT INTO `contact_messages` (`identity`, `apiid`, `uid`, `is_outbox`, `is_read`, `is_saved`, `messagestate`, `sort_by`, `created_at`, `sent_at`, `received_at`, `seen_at`, `modified_at`, `contact_message_type`, `body`, `is_statusmessage`, `is_queued`, `is_sent`, `caption`) VALUES "
											 "(:identity, :apiid, :uid, :isOutbox, :isRead, :isSaved, :messageState, :sortBy, :createdAt, :sentAt, :receivedAt, :seenAt, :modifiedAt, :type, :body, :isStatusMessage, :isQueued, :isSent, :caption);"));
				query.bindValue(QStringLiteral(":identity"), identity);
				query.bindValue(QStringLiteral(":apiid"), apiid);
				query.bindValue(QStringLiteral(":uid"), uid);
				query.bindValue(QStringLiteral(":isOutbox"), isOutbox);
				query.bindValue(QStringLiteral(":isRead"), isRead);
				query.bindValue(QStringLiteral(":isSaved"), isSaved);
				query.bindValue(QStringLiteral(":messageState"), messageState);
				query.bindValue(QStringLiteral(":sortBy"), sortBy);
				query.bindValue(QStringLiteral(":createdAt"), createdAt);
				query.bindValue(QStringLiteral(":sentAt"), sentAt);
				query.bindValue(QStringLiteral(":receivedAt"), receivedAt);
				query.bindValue(QStringLiteral(":seenAt"), seenAt);
				query.bindValue(QStringLiteral(":modifiedAt"), modifiedAt);
				query.bindValue(QStringLiteral(":type"), type);
				query.bindValue(QStringLiteral(":body"), body);
				query.bindValue(QStringLiteral(":isStatusMessage"), isStatusMessage);
				query.bindValue(QStringLiteral(":isQueued"), isQueued);
				query.bindValue(QStringLiteral(":isSent"), isSent);
				query.bindValue(QStringLiteral(":caption"), caption);

				if (!query.execBatch()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not batch-insert contact message data into 'contact_messages'. Query error: " << query.lastError().text().toStdString();
				}

				//if (!database.database.commit()) {
				if (!database->transactionCommit()) {
					LOGGER()->warn("Could NOT commit transaction!");
				}

				auto endTimeBatch = std::chrono::high_resolution_clock::now();
				auto timeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeBatch - startTime).count();
				double timePerMsg = timeInMs;
				timePerMsg /= messages.size();

				LOGGER()->info("Inserting {} contact messages took {}ms, whick makes {}ms per message.", messages.size(), timeInMs, timePerMsg);

				//insertContactMessage(database, message.getContactId(), message.getApiId(), message.getUuid(), message.getIsOutbox(), message.getIsRead(), message.getIsSaved(), message.getMessageState(), message.getCreatedAt(), message.getSentAt(), message.getReceivedAt(), seenAt, message.getModifiedAt(), message.getMessageType(), message.getBody(), message.getIsStatusMessage(), message.getIsQueued(), isSent, message.getCaption());
			}

			QString DatabaseContactMessage::getContentAsText() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::TEXT) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as text because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return queryField(QStringLiteral("body")).toString();
			}

			openmittsu::utility::Location DatabaseContactMessage::getContentAsLocation() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::LOCATION) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as location because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return openmittsu::utility::Location::fromDatabaseString(queryField(QStringLiteral("body")).toString());
			}

			MediaFileItem DatabaseContactMessage::getContentAsImage() const {
				ContactMessageType const messageType = getMessageType();
				if (messageType != ContactMessageType::IMAGE) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as image because it has type " << ContactMessageTypeHelper::toString(messageType) << "!";
				}
				return getMediaItem(getUid());
			}

		}
	}
}