#ifndef OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_

#include <QList>
#include <QString>

#include "src/protocol/ContactId.h"
#include "src/database/DatabaseReadonlyUserMessage.h"
#include "src/dataproviders/messages/ReadonlyContactMessage.h"
#include "src/dataproviders/messages/ContactMessageType.h"

namespace openmittsu {
	namespace backup {
		class ContactMessageBackupObject;
	}

	namespace database {
		class DatabaseReadonlyContactMessage : public virtual DatabaseReadonlyUserMessage, public virtual openmittsu::dataproviders::messages::ReadonlyContactMessage {
		public:
			explicit DatabaseReadonlyUserMessage(openmittsu::protocol::ContactId sender, openmittsu::protocol::MessageId messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime createdAt, openmittsu::protocol::MessageTime sentAt, openmittsu::protocol::MessageTime modifiedAt, bool isQueued, bool isSent, QString uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState messageState, openmittsu::protocol::MessageTime receivedAt, openmittsu::protocol::MessageTime seenAt, bool isStatusMessage, QString caption);
			explicit DatabaseReadonlyContactMessage(DatabaseWrapper const& database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
			virtual ~DatabaseReadonlyContactMessage();

			virtual openmittsu::protocol::ContactId const& getContactId() const override;
			virtual openmittsu::dataproviders::messages::ContactMessageType getMessageType() const override;

			virtual QString getContentAsText() const override;
			virtual openmittsu::utility::Location getContentAsLocation() const override;
			virtual MediaFileItem getContentAsImage() const override;
		private:
			openmittsu::protocol::ContactId const m_contact;
			openmittsu::dataproviders::messages::ContactMessageType m_contactMessageType;
			QString m_body;
			MediaFileItem m_mediaItem;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_
