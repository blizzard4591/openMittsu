#include "src/backup/ContactMessageBackupObject.h"

#include "src/exceptions/IllegalArgumentException.h"

namespace openmittsu {
	namespace backup {

		ContactMessageBackupObject::ContactMessageBackupObject(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool const& isOutbox, bool const& isRead, bool const& isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState,
			openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& modifiedAt, openmittsu::dataproviders::messages::ContactMessageType const& messageType,
			QString const& body, bool const& isStatusMessage, bool const& isQueued, QString const& caption) : BackupObject(),
			m_contact(contact), m_apiId(apiId), m_uuid(uuid), m_isOutbox(isOutbox), m_isRead(isRead), m_isSaved(isSaved), m_messageState(messageState),
			m_createdAt(createdAt), m_sentAt(sentAt), m_receivedAt(receivedAt), m_modifiedAt(modifiedAt), m_messageType(messageType),
			m_body(body), m_isStatusMessage(isStatusMessage), m_isQueued(isQueued), m_caption(caption) {
			//
		}
		
		ContactMessageBackupObject::~ContactMessageBackupObject() {
			//
		}

		ContactMessageBackupObject ContactMessageBackupObject::fromBackupMatch(QString const& filename, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines) {
			QString const contactIdString = filename.mid(8, 8); // skip "message_"
			openmittsu::protocol::ContactId const contact(contactIdString);
			
			return fromBackupMatch(contact, headerOffsets, splittedLines);
		}

		openmittsu::protocol::ContactId const& ContactMessageBackupObject::getContactId() const {
			return m_contact;
		}
		
		openmittsu::protocol::MessageId const& ContactMessageBackupObject::getApiId() const {
			return m_apiId;
		}
		
		QString const& ContactMessageBackupObject::getUuid() const {
			return m_uuid;
		}
		
		bool ContactMessageBackupObject::getIsOutbox() const {
			return m_isOutbox;
		}
		
		bool ContactMessageBackupObject::getIsRead() const {
			return m_isRead;
		}
		
		bool ContactMessageBackupObject::getIsSaved() const {
			return m_isSaved;
		}
		
		openmittsu::dataproviders::messages::UserMessageState const& ContactMessageBackupObject::getMessageState() const {
			return m_messageState;
		}
		
		openmittsu::protocol::MessageTime const& ContactMessageBackupObject::getCreatedAt() const {
			return m_createdAt;
		}
		
		openmittsu::protocol::MessageTime const& ContactMessageBackupObject::getSentAt() const {
			return m_sentAt;
		}
		
		openmittsu::protocol::MessageTime const& ContactMessageBackupObject::getReceivedAt() const {
			return m_receivedAt;
		}
		
		openmittsu::protocol::MessageTime const& ContactMessageBackupObject::getModifiedAt() const {
			return m_modifiedAt;
		}
		
		openmittsu::dataproviders::messages::ContactMessageType const& ContactMessageBackupObject::getMessageType() const {
			return m_messageType;
		}
		
		QString const& ContactMessageBackupObject::getBody() const {
			return m_body;
		}
		
		bool ContactMessageBackupObject::getIsStatusMessage() const {
			return m_isStatusMessage;
		}
		
		bool ContactMessageBackupObject::getIsQueued() const {
			return m_isQueued;
		}

		QString const& ContactMessageBackupObject::getCaption() const {
			return m_caption;
		}

		ContactMessageBackupObject ContactMessageBackupObject::fromBackupMatch(openmittsu::protocol::ContactId const& contact, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines) {
			QSet<QString> const requiredFields = { "apiid","uid","isoutbox","isread","issaved","messagestae","posted_at","created_at","modified_at","type","body","isstatusmessage","isqueued","caption" };
			if (!hasRequiredFields(requiredFields, headerOffsets)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not parse data to Contact message, not all required fields are present!";
			} else {
				openmittsu::protocol::MessageId const apiId(splittedLines.getColumn(headerOffsets.value(QStringLiteral("apiid"))));
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
				openmittsu::dataproviders::messages::ContactMessageType const messageType = openmittsu::dataproviders::messages::ContactMessageTypeHelper::fromString(splittedLines.getColumn(headerOffsets.value(QStringLiteral("type"))));
				QString const body = splittedLines.getColumn(headerOffsets.value(QStringLiteral("body")));
				bool const isStatusMessage = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isstatusmessage"))).toInt() != 0;
				bool const isQueued = splittedLines.getColumn(headerOffsets.value(QStringLiteral("isqueued"))).toInt() != 0;
				QString const caption = splittedLines.getColumn(headerOffsets.value(QStringLiteral("caption")));

				return ContactMessageBackupObject(contact, apiId, uuid, isOutbox, isRead, isSaved, messageState, createdAt, sentAt, receivedAt, modifiedAt, messageType, body, isStatusMessage, isQueued, caption);
			}
		}

		QHash<openmittsu::protocol::ContactId, QString> ContactMessageBackupObject::getContactMessageFiles(QDir const& path) {
			QHash<openmittsu::protocol::ContactId, QString> result;

			QStringList const files = path.entryList(QDir::Files);
			for (int i = 0; i < files.size(); ++i) {
				QString const& filename = files.at(i);
				if (filename.startsWith(QStringLiteral("message_")) && (filename.size() == 20) && filename.endsWith(QStringLiteral(".csv"))) {
					QString const idString = filename.mid(8, 8);
					result.insert(openmittsu::protocol::ContactId(idString), filename);
				}
			}

			return result;
		}

	}
}
