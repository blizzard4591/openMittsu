#include "src/database/DatabaseUserMessage.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		DatabaseUserMessage::DatabaseUserMessage(InternalDatabaseInterface* database, openmittsu::protocol::MessageId const& messageId) : DatabaseMessage(database, messageId), UserMessage() {
			//
		}

		DatabaseUserMessage::~DatabaseUserMessage() {
			//
		}

		bool DatabaseUserMessage::isRead() const {
			return queryField(QStringLiteral("is_read")).toBool();
		}

		bool DatabaseUserMessage::isSaved() const {
			return queryField(QStringLiteral("is_saved")).toBool();
		}

		openmittsu::protocol::MessageTime DatabaseUserMessage::getReceivedAt() const {
			return openmittsu::protocol::MessageTime::fromDatabase(queryField(QStringLiteral("received_at")).toLongLong());
		}

		openmittsu::protocol::MessageTime DatabaseUserMessage::getSeenAt() const {
			return openmittsu::protocol::MessageTime::fromDatabase(queryField(QStringLiteral("seen_at")).toLongLong());
		}

		bool DatabaseUserMessage::isStatusMessage() const {
			return queryField(QStringLiteral("is_statusmessage")).toBool();
		}

		QString DatabaseUserMessage::getCaption() const {
			return queryField(QStringLiteral("caption")).toString();
		}

		UserMessageState DatabaseUserMessage::getMessageState() const {
			return UserMessageStateHelper::fromString(queryField(QStringLiteral("messagestate")).toString());
		}

		void DatabaseUserMessage::setMessageState(UserMessageState const& messageState, openmittsu::protocol::MessageTime const& when) {
			UserMessageState const currentState = getMessageState();
			if (!UserMessageStateHelper::canSwitchTo(currentState, messageState)) {
				LOGGER()->warn("Can not change message state from \"{}\" to \"{}\" on message #{}.", UserMessageStateHelper::toString(currentState).toStdString(), UserMessageStateHelper::toString(messageState).toStdString(), getMessageId().toString());
			} else {
				QVariantMap fieldsAndValues;

				fieldsAndValues.insert(QStringLiteral("messagestate"), QVariant(UserMessageStateHelper::toString(messageState)));
				fieldsAndValues.insert(QStringLiteral("modified_at"), QVariant(when.getMessageTimeMSecs()));
				if ((messageState != UserMessageState::SENDFAILED) && (messageState != UserMessageState::SENDING)) {
					fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(1));
					fieldsAndValues.insert(QStringLiteral("is_sent"), QVariant(1));
					if (getSentAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("sent_at"), QVariant(when.getMessageTimeMSecs()));
					}
				}


				if (messageState == UserMessageState::SENDFAILED) {
					fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(0));
				} else if (messageState == UserMessageState::SENDING) {
					fieldsAndValues.insert(QStringLiteral("is_queued"), QVariant(1));
				} else if (messageState == UserMessageState::SENT) {
					//setField(QStringLiteral("sent_at"), QVariant(when.getMessageTimeMSecs()));
				} else if (messageState == UserMessageState::DELIVERED) {
					fieldsAndValues.insert(QStringLiteral("received_at"), QVariant(when.getMessageTimeMSecs()));
				} else if (messageState == UserMessageState::READ) {
					fieldsAndValues.insert(QStringLiteral("seen_at"), QVariant(when.getMessageTimeMSecs()));
					fieldsAndValues.insert(QStringLiteral("is_read"), QVariant(1));
					if (getReceivedAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("received_at"), QVariant(when.getMessageTimeMSecs()));
					}
				} else if (messageState == UserMessageState::USERACK) {
					fieldsAndValues.insert(QStringLiteral("is_read"), QVariant(1));
					if (getReceivedAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("received_at"), QVariant(when.getMessageTimeMSecs()));
					}
					if (getSeenAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("seen_at"), QVariant(when.getMessageTimeMSecs()));
					}
				} else if (messageState == UserMessageState::USERDEC) {
					fieldsAndValues.insert(QStringLiteral("is_read"), QVariant(1));
					if (getReceivedAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("received_at"), QVariant(when.getMessageTimeMSecs()));
					}
					if (getSeenAt().isNull()) {
						fieldsAndValues.insert(QStringLiteral("seen_at"), QVariant(when.getMessageTimeMSecs()));
					}
				} else {
					LOGGER()->warn("Unhandled new message state \"{}\" on message #{}.", static_cast<int>(messageState), getMessageId().toString());
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled message state: " << static_cast<int>(messageState);
				}

				setFields(fieldsAndValues);
			}
		}

		void DatabaseUserMessage::setIsSent() {
			setMessageState(UserMessageState::SENT, openmittsu::protocol::MessageTime::now());
		}

	}
}
