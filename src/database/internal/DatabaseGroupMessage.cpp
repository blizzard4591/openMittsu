#include "src/database/internal/DatabaseGroupMessage.h"

#include "src/backup/GroupMessageBackupObject.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseGroupMessage::DatabaseGroupMessage(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) : DatabaseMessage(database, messageId), DatabaseUserMessage(database, messageId), GroupMessage(), m_group(group) {
				if (!exists(database, group, messageId)) {
					throw openmittsu::exceptions::InternalErrorException() << "No message from group \"" << group.toString() << "\" and message ID \"" << messageId.toString() << "\" exists, can not manipulate.";
				}
			}

			DatabaseGroupMessage::~DatabaseGroupMessage() {
				//
			}

			openmittsu::protocol::GroupId const& DatabaseGroupMessage::getGroupId() const {
				return m_group;
			}

			int DatabaseGroupMessage::getGroupMessageCount(InternalDatabaseInterface const* database) {
				return openmittsu::database::internal::DatabaseUtilities::countQuery(database, QStringLiteral("group_messages"));
			}

			int DatabaseGroupMessage::getGroupMessageCount(InternalDatabaseInterface const* database, openmittsu::protocol::GroupId const& group) {
				return openmittsu::database::internal::DatabaseUtilities::countQuery(database, QStringLiteral("group_messages"), { { QStringLiteral("group_id"), group.groupIdWithoutOwnerToQString() }, { QStringLiteral("group_creator"), group.getOwner().toQString() } });
			}

			bool DatabaseGroupMessage::exists(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `apiid` FROM `group_messages` WHERE `group_id` = :groupId AND `group_creator` = :groupCreator AND `apiid` = :apiid;"));
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(messageId.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group message existance query for table contact_messages for group \"" << group.toString() << "\" and message ID \"" << messageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}
				return query.next();
			}

			GroupMessageType DatabaseGroupMessage::getMessageType() const {
				return GroupMessageTypeHelper::fromString(queryField(QStringLiteral("group_message_type")).toString());
			}

			QString DatabaseGroupMessage::getWhereString() const {
				return QStringLiteral("`group_id` = :groupId AND `group_creator` = :groupCreator");
			}

			void DatabaseGroupMessage::bindWhereStringValues(QSqlQuery& query) const {
				query.bindValue(QStringLiteral(":groupId"), QVariant(m_group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(m_group.getOwner().toQString()));
			}

			QString DatabaseGroupMessage::getTableName() const {
				return QStringLiteral("group_messages");
			}

			void DatabaseGroupMessage::insertGroupMessage(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool isOutgoing, bool isRead, bool isSaved, UserMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, openmittsu::protocol::MessageTime const& modifiedAt, GroupMessageType const& type, QString const& body, bool isStatusMessage, bool isQueued, bool isSent, QString const& caption) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("INSERT INTO `group_messages` (`group_id`, `group_creator`, `apiid`, `uid`, `identity`, `is_outbox`, `is_read`, `is_saved`, `messagestate`, `sort_by`, `created_at`, `sent_at`, `received_at`, `seen_at`, `modified_at`, `group_message_type`, `body`, `is_statusmessage`, `is_queued`, `is_sent`, `caption`) VALUES "
											 "(:groupId, :groupCreator, :apiid, :uid, :identity, :isOutbox, :isRead, :isSaved, :messageState, :sortBy, :createdAt, :sentAt, :receivedAt, :seenAt, :modifiedAt, :type, :body, :isStatusMessage, :isQueued, :isSent, :caption);"));
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(apiId.toQString()));
				query.bindValue(QStringLiteral(":uid"), QVariant(uuid));
				query.bindValue(QStringLiteral(":identity"), QVariant(sender.toQString()));
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
				query.bindValue(QStringLiteral(":type"), QVariant(GroupMessageTypeHelper::toQString(type)));
				query.bindValue(QStringLiteral(":body"), QVariant(body));
				query.bindValue(QStringLiteral(":isStatusMessage"), QVariant(isStatusMessage));
				query.bindValue(QStringLiteral(":isQueued"), QVariant(isQueued));
				query.bindValue(QStringLiteral(":isSent"), QVariant(isSent));
				query.bindValue(QStringLiteral(":caption"), QVariant(caption));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert group message data into 'group_messages'. Query error: " << query.lastError().text().toStdString();
				}
			}

			bool DatabaseGroupMessage::resetQueueStatus(InternalDatabaseInterface* database, int maxAgeInSeconds) {
				QSqlQuery query(database->getQueryObject());

				openmittsu::protocol::MessageTime const limit(QDateTime::currentDateTime().addSecs(-maxAgeInSeconds));
				query.prepare(QStringLiteral("SELECT `group_id`, `group_creator`, `apiid`, `uid` FROM `group_messages` WHERE `is_outbox` = 1 AND `is_queued` = 1 AND `is_sent` = 0 AND ((`modified_at` <= %1) OR (`modified_at` IS NULL));").arg(limit.getMessageTimeMSecs()));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group message queue reset query for table group_messages. Query error: " << query.lastError().text().toStdString();
				}

				bool hasAtLeastOneEntry = false;
				while (query.next()) {
					hasAtLeastOneEntry = true;
					openmittsu::protocol::GroupId const group(openmittsu::protocol::ContactId(query.value(QStringLiteral("group_creator")).toString()), query.value(QStringLiteral("group_id")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					DatabaseGroupMessage message(database, group, messageId);
					message.setIsQueued(false);
				}

				return hasAtLeastOneEntry;
			}

			openmittsu::protocol::MessageId DatabaseGroupMessage::insertGroupMessageFromUs(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, QString const& uuid, openmittsu::protocol::MessageTime const& createdAt, GroupMessageType const& type, QString const& body, bool isQueued, bool isStatusMessage, QString const& caption) {
				openmittsu::protocol::MessageId const messageId = database->getNextMessageId(group);

				insertGroupMessage(database, group, database->getSelfContact(), messageId, uuid, true, false, true, UserMessageState::SENDING, createdAt, openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), type, body, isStatusMessage, isQueued, false, caption);

				database->announceNewMessage(group, uuid);

				return messageId;
			}

			void DatabaseGroupMessage::insertGroupMessageFromThem(InternalDatabaseInterface* database, openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, QString const& uuid, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, GroupMessageType const& type, QString const& body, bool isStatusMessage, QString const& caption) {
				insertGroupMessage(database, group, sender, messageId, uuid, false, false, true, UserMessageState::DELIVERED, sentAt, sentAt, receivedAt, openmittsu::protocol::MessageTime(), openmittsu::protocol::MessageTime(), type, body, isStatusMessage, true, true, caption);

				database->announceNewMessage(group, uuid);
				database->announceReceivedNewMessage(group);
			}

			void DatabaseGroupMessage::insertGroupMessagesFromBackup(InternalDatabaseInterface* database, QList<openmittsu::backup::GroupMessageBackupObject> const& messages) {
				auto startTime = std::chrono::high_resolution_clock::now();
				QVariantList groupId;
				groupId.reserve(messages.size());
				QVariantList groupCreator;
				groupCreator.reserve(messages.size());
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
					groupId.append(it->getGroupId().groupIdWithoutOwnerToQString());
					groupCreator.append(it->getGroupId().getOwner().toQString());
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
					type.append(GroupMessageTypeHelper::toQString(it->getMessageType()));
					body.append(it->getBody());
					isStatusMessage.append(it->getIsStatusMessage());
					isQueued.append(it->getIsQueued());
					isSent.append(UserMessageStateHelper::isSent(it->getMessageState()));
					caption.append(it->getCaption());
				}

				if (!database->transactionStart()) {
					LOGGER()->warn("Could NOT start transaction!");
				}

				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("INSERT INTO `group_messages` (`group_id`, `group_creator`, `apiid`, `uid`, `identity`, `is_outbox`, `is_read`, `is_saved`, `messagestate`, `sort_by`, `created_at`, `sent_at`, `received_at`, `seen_at`, `modified_at`, `group_message_type`, `body`, `is_statusmessage`, `is_queued`, `is_sent`, `caption`) VALUES "
											 "(:groupId, :groupCreator, :apiid, :uid, :identity, :isOutbox, :isRead, :isSaved, :messageState, :sortBy, :createdAt, :sentAt, :receivedAt, :seenAt, :modifiedAt, :type, :body, :isStatusMessage, :isQueued, :isSent, :caption);"));
				query.bindValue(QStringLiteral(":groupId"), groupId);
				query.bindValue(QStringLiteral(":groupCreator"), groupCreator);
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
					throw openmittsu::exceptions::InternalErrorException() << "Could not batch-insert group message data into 'group_messages'. Query error: " << query.lastError().text().toStdString();
				}
				// insertGroupMessage(database, message.getGroupId(), message.getContactId(), message.getApiId(), message.getUuid(), message.getIsOutbox(), message.getIsRead(), message.getIsSaved(), message.getMessageState(), message.getCreatedAt(), message.getSentAt(), message.getReceivedAt(), seenAt, message.getModifiedAt(), message.getMessageType(), message.getBody(), message.getIsStatusMessage(), message.getIsQueued(), isSent, message.getCaption());

				if (!database->transactionCommit()) {
					LOGGER()->info("Could NOT commit transaction!");
				}
				auto endTime = std::chrono::high_resolution_clock::now();
				auto timeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
				double timePerMsg = timeInMs;
				timePerMsg /= messages.size();

				LOGGER()->info("Inserting {} group messages took {}ms, whick makes {}ms per message.", messages.size(), timeInMs, timePerMsg);
			}

			QString DatabaseGroupMessage::getContentAsText() const {
				GroupMessageType const messageType = getMessageType();
				if ((messageType != GroupMessageType::TEXT) && (messageType != GroupMessageType::SET_IMAGE) && (messageType != GroupMessageType::SET_TITLE) && (messageType != GroupMessageType::GROUP_CREATION) && (messageType != GroupMessageType::LEAVE) && (messageType != GroupMessageType::SYNC_REQUEST)) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as text because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
				}
				return queryField(QStringLiteral("body")).toString();
			}

			openmittsu::utility::Location DatabaseGroupMessage::getContentAsLocation() const {
				GroupMessageType const messageType = getMessageType();
				if (messageType != GroupMessageType::LOCATION) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as location because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
				}
				return openmittsu::utility::Location::fromDatabaseString(queryField(QStringLiteral("body")).toString());
			}

			MediaFileItem DatabaseGroupMessage::getContentAsImage() const {
				GroupMessageType const messageType = getMessageType();
				if (messageType != GroupMessageType::IMAGE) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not get content of message for message ID \"" << getMessageId().toString() << "\" as image because it has type " << GroupMessageTypeHelper::toString(messageType) << "!";
				}
				return getMediaItem(getUid());
			}

		}
	}
}
