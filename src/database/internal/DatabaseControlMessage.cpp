#include "src/database/internal/DatabaseControlMessage.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {
		namespace internal {

			using namespace openmittsu::dataproviders::messages;

			DatabaseControlMessage::DatabaseControlMessage(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& controlMessageId, openmittsu::protocol::MessageId const& relatedMessageId, ControlMessageType const& controlMessageType) : DatabaseMessage(database, controlMessageId), ControlMessage(), m_contact(contact), m_controlMessageType(controlMessageType), m_relatedMessageId(relatedMessageId) {
				if (!hasControlMessageFor(database, contact, relatedMessageId, m_controlMessageType)) {
					throw openmittsu::exceptions::InternalErrorException() << "No control message from contact \"" << contact.toString() << "\" and message ID \"" << relatedMessageId.toString() << "\" exists for type " << ControlMessageTypeHelper::toString(m_controlMessageType).toStdString() << ", can not manipulate.";
				}
			}

			DatabaseControlMessage::~DatabaseControlMessage() {
				//
			}

			openmittsu::protocol::ContactId const& DatabaseControlMessage::getContactId() const {
				return m_contact;
			}

			ControlMessageType DatabaseControlMessage::getControlMessageType() const {
				return m_controlMessageType;
			}

			bool DatabaseControlMessage::exists(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `apiid` FROM `control_messages` WHERE `identity` = :identity AND `apiid` = :apiid;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(messageId.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute control message existance query for table control_messages for identity \"" << contact.toString() << "\" and message ID \"" << messageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}
				return query.next();
			}

			bool DatabaseControlMessage::hasControlMessageFor(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& relatedMessageId, ControlMessageType const& controlMessageType) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `apiid` FROM `control_messages` WHERE `identity` = :identity AND `related_message_apiid` = :relatedMessageId AND `control_message_type` = :controlType;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":relatedMessageId"), QVariant(relatedMessageId.toQString()));
				query.bindValue(QStringLiteral(":controlType"), QVariant(ControlMessageTypeHelper::toString(controlMessageType)));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute control message existance query for table control_messages for identity \"" << contact.toString() << "\" and message ID \"" << relatedMessageId.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}
				return query.next();
			}

			openmittsu::protocol::MessageId DatabaseControlMessage::insertControlMessageFromUs(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& relatedMessageId, openmittsu::dataproviders::messages::ControlMessageState const& messageState, openmittsu::protocol::MessageTime const& createdAt, bool isQueued, ControlMessageType const& controlMessageType) {
				openmittsu::protocol::MessageId const messageId = database->getNextMessageId(contact);
				QString const uuid = database->generateUuid();

				QSqlQuery query(database->getQueryObject());

				query.prepare(QStringLiteral("INSERT INTO `control_messages` (`identity`, `apiid`, `related_message_apiid`, `uid`, `is_outbox`, `messagestate`, `created_at`, `modified_at`, `control_message_type`, `is_queued`, `is_sent`) VALUES "
											 "(:identity, :apiid, :relatedMessageApiid, :uid, :isOutbox, :messageState, :createdAt, :modifiedAt, :controlType, :isQueued, :isSent);"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(messageId.toQString()));
				query.bindValue(QStringLiteral(":relatedMessageApiid"), QVariant(relatedMessageId.toQString()));
				query.bindValue(QStringLiteral(":messageState"), QVariant(ControlMessageStateHelper::toString(messageState)));
				query.bindValue(QStringLiteral(":uid"), QVariant(uuid));
				query.bindValue(QStringLiteral(":isOutbox"), QVariant(1));
				query.bindValue(QStringLiteral(":createdAt"), QVariant(createdAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":modifiedAt"), QVariant());
				query.bindValue(QStringLiteral(":controlType"), QVariant(ControlMessageTypeHelper::toString(controlMessageType)));
				query.bindValue(QStringLiteral(":isQueued"), QVariant(isQueued));
				query.bindValue(QStringLiteral(":isSent"), QVariant(0));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert contact message data into 'contact_messages'. Query error: " << query.lastError().text().toStdString();
				}

				return messageId;
			}

			bool DatabaseControlMessage::resetQueueStatus(InternalDatabaseInterface* database, int maxAgeInSeconds) {
				QSqlQuery query(database->getQueryObject());

				openmittsu::protocol::MessageTime const limit(QDateTime::currentDateTime().addSecs(-maxAgeInSeconds));
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `related_message_apiid`, `uid`, `control_message_type` FROM `control_messages` WHERE `is_outbox` = 1 AND `is_queued` = 1 AND `is_sent` = 0 AND ((`modified_at` <= %1) OR (`modified_at` IS NULL));").arg(limit.getMessageTimeMSecs()));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute control message queue reset query for table control_messages. Query error: " << query.lastError().text().toStdString();
				}

				bool hasAtLeastOneEntry = false;
				while (query.next()) {
					hasAtLeastOneEntry = true;
					ControlMessageType const messageType = ControlMessageTypeHelper::fromString(query.value(QStringLiteral("control_message_type")).toString());
					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					openmittsu::protocol::MessageId const relatedMessageId(query.value(QStringLiteral("related_message_apiid")).toString());
					DatabaseControlMessage message(database, receiver, messageId, relatedMessageId, messageType);
					message.setIsQueued(false);
				}

				return hasAtLeastOneEntry;
			}

			QString DatabaseControlMessage::getWhereString() const {
				return QStringLiteral("`identity` = :identity");
			}

			void DatabaseControlMessage::bindWhereStringValues(QSqlQuery& query) const {
				query.bindValue(QStringLiteral(":identity"), QVariant(m_contact.toQString()));
			}

			QString DatabaseControlMessage::getTableName() const {
				return QStringLiteral("control_messages");
			}

			ControlMessageState DatabaseControlMessage::getMessageState() const {
				return ControlMessageStateHelper::fromString(queryField(QStringLiteral("messagestate")).toString());
			}

			void DatabaseControlMessage::setMessageState(ControlMessageState const& messageState, openmittsu::protocol::MessageTime const& when) {
				ControlMessageState const currentState = getMessageState();
				if (!ControlMessageStateHelper::canSwitchTo(currentState, messageState)) {
					LOGGER()->warn("Can not change message state from \"{}\" to \"{}\" on message #{} with timestamp {}.", ControlMessageStateHelper::toString(currentState).toStdString(), ControlMessageStateHelper::toString(messageState).toStdString(), getMessageId().toString(), when.toString());
				} else {
					QVariantMap fieldsAndValues;

					fieldsAndValues.insert(QStringLiteral("messagestate"), QVariant(ControlMessageStateHelper::toString(messageState)));
					fieldsAndValues.insert(QStringLiteral("modified_at"), QVariant(when.getMessageTimeMSecs()));
					if ((messageState != ControlMessageState::SENDFAILED) && (messageState != ControlMessageState::SENDING)) {
						fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(1));
						fieldsAndValues.insert(QStringLiteral("is_sent"), QVariant(1));
						if (getSentAt().isNull()) {
							fieldsAndValues.insert(QStringLiteral("sent_at"), QVariant(when.getMessageTimeMSecs()));
						}
					}


					if (messageState == ControlMessageState::SENDFAILED) {
						fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(0));
					} else if (messageState == ControlMessageState::SENDING) {
						fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(1));
					} else if (messageState == ControlMessageState::SENT) {
						//setField(QStringLiteral("sent_at"), QVariant(when.getMessageTimeMSecs()));
					} else {
						LOGGER()->warn("Unhandled new message state \"{}\" on message #{}.", static_cast<int>(messageState), getMessageId().toString());
						throw openmittsu::exceptions::InternalErrorException() << "Unhandled message state: " << static_cast<int>(messageState);
					}

					setFields(fieldsAndValues);
				}
			}

			DatabaseControlMessage DatabaseControlMessage::fromUuid(InternalDatabaseInterface* database, QString const& uuid) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `related_message_apiid`, `uid`, `control_message_type` FROM `control_messages` WHERE `uid` = :uid;"));
				query.bindValue(QStringLiteral(":uid"), QVariant(uuid));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute control message query for table control_messages for UUID \"" << uuid.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
				}

				if (query.next()) {
					ControlMessageType const messageType = ControlMessageTypeHelper::fromString(query.value(QStringLiteral("control_message_type")).toString());
					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					openmittsu::protocol::MessageId const relatedMessageId(query.value(QStringLiteral("related_message_apiid")).toString());
					return DatabaseControlMessage(database, receiver, messageId, relatedMessageId, messageType);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "There is no control message in table control_messages for UUID \"" << uuid.toStdString() << "\".";
				}
			}

			DatabaseControlMessage DatabaseControlMessage::fromReceiverAndControlMessageId(InternalDatabaseInterface* database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& controlMessageId) {
				QSqlQuery query(database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `related_message_apiid`, `uid`, `control_message_type` FROM `control_messages` WHERE `identity` = :identity AND `apiid` = :apiid;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":apiid"), QVariant(controlMessageId.toQString()));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute control message query for table control_messages for contact \"" << contact.toString() << "\" and control message id #" << controlMessageId.toString() << ". Query error: " << query.lastError().text().toStdString();
				}

				if (query.next()) {
					ControlMessageType const messageType = ControlMessageTypeHelper::fromString(query.value(QStringLiteral("control_message_type")).toString());
					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					openmittsu::protocol::MessageId const relatedMessageId(query.value(QStringLiteral("related_message_apiid")).toString());
					return DatabaseControlMessage(database, receiver, messageId, relatedMessageId, messageType);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "There is no control message in table control_messages for contact \"" << contact.toString() << "\" and control message id #" << controlMessageId.toString() << ".";
				}
			}

		}
	}
}
