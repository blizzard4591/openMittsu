#include "src/backup/GroupMessageBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace backup {

		GroupMessageBackupObject::GroupMessageBackupObject(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool const& isOutbox, bool const& isRead, bool const& isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState,
			openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& modifiedAt, openmittsu::dataproviders::messages::GroupMessageType const& messageType,
			QString const& body, bool const& isStatusMessage, bool const& isQueued, QString const& caption) : BackupObject(),
			m_group(groupId), m_contact(contact), m_apiId(apiId), m_uuid(uuid), m_isOutbox(isOutbox), m_isRead(isRead), m_isSaved(isSaved), m_messageState(messageState),
			m_createdAt(createdAt), m_sentAt(sentAt), m_receivedAt(receivedAt), m_modifiedAt(modifiedAt), m_messageType(messageType),
			m_body(body), m_isStatusMessage(isStatusMessage), m_isQueued(isQueued), m_caption(caption) {
			//
		}
		
		GroupMessageBackupObject::~GroupMessageBackupObject() {
			//
		}

		openmittsu::protocol::GroupId const& GroupMessageBackupObject::getGroupId() const {
			return m_group;
		}

		openmittsu::protocol::ContactId const& GroupMessageBackupObject::getContactId() const {
			return m_contact;
		}

		openmittsu::protocol::MessageId const& GroupMessageBackupObject::getApiId() const {
			return m_apiId;
		}

		QString const& GroupMessageBackupObject::getUuid() const {
			return m_uuid;
		}

		bool GroupMessageBackupObject::getIsOutbox() const {
			return m_isOutbox;
		}

		bool GroupMessageBackupObject::getIsRead() const {
			return m_isRead;
		}

		bool GroupMessageBackupObject::getIsSaved() const {
			return m_isSaved;
		}

		openmittsu::dataproviders::messages::UserMessageState const& GroupMessageBackupObject::getMessageState() const {
			return m_messageState;
		}

		openmittsu::protocol::MessageTime const& GroupMessageBackupObject::getCreatedAt() const {
			return m_createdAt;
		}

		openmittsu::protocol::MessageTime const& GroupMessageBackupObject::getSentAt() const {
			return m_sentAt;
		}

		openmittsu::protocol::MessageTime const& GroupMessageBackupObject::getReceivedAt() const {
			return m_receivedAt;
		}

		openmittsu::protocol::MessageTime const& GroupMessageBackupObject::getModifiedAt() const {
			return m_modifiedAt;
		}

		openmittsu::dataproviders::messages::GroupMessageType const& GroupMessageBackupObject::getMessageType() const {
			return m_messageType;
		}

		QString const& GroupMessageBackupObject::getBody() const {
			return m_body;
		}

		bool GroupMessageBackupObject::getIsStatusMessage() const {
			return m_isStatusMessage;
		}

		bool GroupMessageBackupObject::getIsQueued() const {
			return m_isQueued;
		}

		QString const& GroupMessageBackupObject::getCaption() const {
			return m_caption;
		}

		GroupMessageBackupObject GroupMessageBackupObject::fromBackupMatch(QString const& filename, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines) {
			if ((filename.size() != (14 + 16 + 1 + 8 + 4)) || (!filename.startsWith(QStringLiteral("group_message_"))) || (!filename.endsWith(QStringLiteral(".csv")))) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse data to Group message, file name does not fit pattern!";
			}

			QString const groupIdStringId = filename.mid(14, 16); // skip "group_message_", capture "2cbaea5413d26198"
			QString const groupIdStringOwner = filename.mid(31, 8); // skip "group_message_2cbaea5413d26198-ECHOECHO"
			openmittsu::protocol::ContactId const groupOwner(groupIdStringOwner);
			openmittsu::protocol::GroupId const groupId(groupOwner, groupIdStringId);

			QSet<QString> const requiredFields = { "apiid","uid","identity","isoutbox","isread","issaved","messagestae","posted_at","created_at","modified_at","type","body","isstatusmessage","isqueued","caption" };
			if (!hasRequiredFields(requiredFields, headerOffsets)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse data to Group message, not all required fields are present!";
			} else {
				QString const& identityString = splittedLines.getColumn(headerOffsets.value(QStringLiteral("identity")));
				openmittsu::protocol::ContactId const contact(identityString.isEmpty() ? groupIdStringOwner : identityString);

				QString const& apiIdString = splittedLines.getColumn(headerOffsets.value(QStringLiteral("apiid")));
				openmittsu::protocol::MessageId const apiId(apiIdString.isEmpty() ? openmittsu::protocol::MessageId::random().toQString() : apiIdString);
				QString const uuid(splittedLines.getColumn(headerOffsets.value(QStringLiteral("uid"))));
				bool const isOutbox = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isoutbox"))).toInt() != 0;
				bool const isRead = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isread"))).toInt() != 0;
				bool const isSaved = splittedLines.getColumn(headerOffsets.value(QStringLiteral("issaved"))).toInt() != 0;

				QString const messageStateString = splittedLines.getColumn(headerOffsets.value(QStringLiteral("messagestae")));
				openmittsu::dataproviders::messages::UserMessageState const messageState = isOutbox ? openmittsu::dataproviders::messages::UserMessageStateHelper::fromString(messageStateString) : (isRead ? openmittsu::dataproviders::messages::UserMessageState::READ : openmittsu::dataproviders::messages::UserMessageState::DELIVERED);
				openmittsu::protocol::MessageTime const createdAt = openmittsu::protocol::MessageTime::fromDatabase(splittedLines.getColumn(headerOffsets.value(QStringLiteral("posted_at"))).toLongLong());

				openmittsu::protocol::MessageTime const sentAt = openmittsu::protocol::MessageTime::fromDatabase(isOutbox ? splittedLines.getColumn(headerOffsets.value(QStringLiteral("created_at"))).toLongLong() : createdAt.getMessageTimeMSecs());
				openmittsu::protocol::MessageTime const receivedAt = openmittsu::protocol::MessageTime::fromDatabase(splittedLines.getColumn(headerOffsets.value(QStringLiteral("created_at"))).toLongLong());

				openmittsu::protocol::MessageTime const modifiedAt = openmittsu::protocol::MessageTime::fromDatabase(splittedLines.getColumn(headerOffsets.value(QStringLiteral("modified_at"))).toLongLong());
				openmittsu::dataproviders::messages::GroupMessageType const messageType = openmittsu::dataproviders::messages::GroupMessageTypeHelper::fromString(splittedLines.getColumn(headerOffsets.value(QStringLiteral("type"))));
				QString const body = splittedLines.getColumn(headerOffsets.value(QStringLiteral("body")));
				bool const isStatusMessage = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isstatusmessage"))).toInt() != 0;
				bool const isQueued = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isqueued"))).toInt() != 0;
				QString const caption = splittedLines.getColumn(headerOffsets.value(QStringLiteral("caption")));

				return GroupMessageBackupObject(groupId, contact, apiId, uuid, isOutbox, isRead, isSaved, messageState, createdAt, sentAt, receivedAt, modifiedAt, messageType, body, isStatusMessage, isQueued, caption);
			}
		}

		QHash<openmittsu::protocol::GroupId, QString> GroupMessageBackupObject::getGroupMessageFiles(QDir const& path) {
			QHash<openmittsu::protocol::GroupId, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if (filename.startsWith(QStringLiteral("group_message_")) && (filename.size() == (14 + 16 + 1 + 8 + 4)) && filename.endsWith(QStringLiteral(".csv"))) {
					QString const groupIdStringId = filename.mid(14, 16);
					QString const groupIdStringOwner = filename.mid(31, 8);
					openmittsu::protocol::ContactId const groupOwner(groupIdStringOwner);
					openmittsu::protocol::GroupId const groupId(groupOwner, groupIdStringId);
					result.insert(groupId, filename);
				}
			}

			return result;
		}

	}
}
