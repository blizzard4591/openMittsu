#ifndef OPENMITTSU_BACKUP_GROUPMESSAGEBACKUPOBJECT_H_
#define OPENMITTSU_BACKUP_GROUPMESSAGEBACKUPOBJECT_H_

#include "src/backup/BackupObject.h"
#include "src/backup/SimpleCsvLineSplitter.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/dataproviders/messages/UserMessageState.h"
#include "src/dataproviders/messages/GroupMessageType.h"

#include <QDir>
#include <QHash>
#include <QString>

namespace openmittsu {
	namespace backup {

		class GroupMessageBackupObject : public BackupObject {
		public:
			GroupMessageBackupObject(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& apiId, QString const& uuid, bool const& isOutbox, bool const& isRead, bool const& isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState,
				openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& modifiedAt, openmittsu::dataproviders::messages::GroupMessageType const& messageType,
				QString const& body, bool const& isStatusMessage, bool const& isQueued, QString const& caption);
			virtual ~GroupMessageBackupObject();

			openmittsu::protocol::GroupId const& getGroupId() const;
			openmittsu::protocol::ContactId const& getContactId() const;
			openmittsu::protocol::MessageId const& getApiId() const;
			QString const& getUuid() const;
			bool getIsOutbox() const;
			bool getIsRead() const;
			bool getIsSaved() const;
			openmittsu::dataproviders::messages::UserMessageState const& getMessageState() const;
			openmittsu::protocol::MessageTime const& getCreatedAt() const;
			openmittsu::protocol::MessageTime const& getSentAt() const;
			openmittsu::protocol::MessageTime const& getReceivedAt() const;
			openmittsu::protocol::MessageTime const& getModifiedAt() const;
			openmittsu::dataproviders::messages::GroupMessageType const& getMessageType() const;
			QString const& getBody() const;
			bool getIsStatusMessage() const;
			bool getIsQueued() const;
			QString const& getCaption() const;

			static GroupMessageBackupObject fromBackupMatch(QString const& filename, QHash<QString, int> const& headerOffsets, SimpleCsvLineSplitter const& splittedLines);
			static QHash<openmittsu::protocol::GroupId, QString> getGroupMessageFiles(QDir const& path);
		private:
			// "apiid","uid","identity","isoutbox","isread","issaved","messagestae","posted_at","created_at","modified_at","type","body","isstatusmessage","isqueued","caption"
			openmittsu::protocol::GroupId const m_group;
			openmittsu::protocol::ContactId const m_contact;
			openmittsu::protocol::MessageId const m_apiId;
			QString const m_uuid;
			bool const m_isOutbox;
			bool const m_isRead;
			bool const m_isSaved;
			openmittsu::dataproviders::messages::UserMessageState const m_messageState;
			openmittsu::protocol::MessageTime const m_createdAt;
			openmittsu::protocol::MessageTime const m_sentAt;
			openmittsu::protocol::MessageTime const m_receivedAt;
			openmittsu::protocol::MessageTime const m_modifiedAt;
			openmittsu::dataproviders::messages::GroupMessageType const m_messageType;
			QString const m_body;
			bool const m_isStatusMessage;
			bool const m_isQueued;
			QString const m_caption;
		};

	}
}

#endif // OPENMITTSU_BACKUP_GROUPMESSAGEBACKUPOBJECT_H_
