#ifndef OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_

#include <QList>
#include <QString>

#include "src/dataproviders/messages/ReadonlyContactMessage.h"
#include "src/dataproviders/messages/ContactMessageType.h"

namespace openmittsu {
	namespace backup {
		class ContactMessageBackupObject;
	}

	namespace database {
		class DatabaseReadonlyContactMessage : public virtual openmittsu::dataproviders::messages::ReadonlyContactMessage {
		public:
			DatabaseReadonlyContactMessage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, bool isMessageFromUs, openmittsu::protocol::MessageTime const& createdAt, openmittsu::protocol::MessageTime const& sentAt, openmittsu::protocol::MessageTime const& modifiedAt, bool isQueued, bool isSent, QString const& uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState const& messageState, openmittsu::protocol::MessageTime const& receivedAt, openmittsu::protocol::MessageTime const& seenAt, bool isStatusMessage, QString const& caption, openmittsu::dataproviders::messages::ContactMessageType const& contactMessageType, QString const& body, MediaFileItem const& mediaItem);
			//explicit DatabaseReadonlyContactMessage(DatabaseWrapper const& database, openmittsu::protocol::ContactId const& contact, openmittsu::protocol::MessageId const& messageId);
			virtual ~DatabaseReadonlyContactMessage();

			virtual openmittsu::protocol::ContactId const& getSender() const override;
			virtual openmittsu::protocol::MessageId const& getMessageId() const override;

			virtual bool isMessageFromUs() const override;

			virtual openmittsu::protocol::MessageTime const& getCreatedAt() const override;
			virtual openmittsu::protocol::MessageTime const& getSentAt() const override;
			virtual openmittsu::protocol::MessageTime const& getModifiedAt() const override;

			/** Threema Meaning unknown. We use it as follows: First, a message is only stored in the database. When we attempt to send it to the server, it is "queued". If that fails, it goes back to not queued. Else, it moves to queued and sent. */
			virtual bool isQueued() const override;

			/** Encodes whether this message has been successfully sent to the server. If this flag is false, the message should be resent on reconnect or timeouts. */
			virtual bool isSent() const override;

			virtual QString const& getUid() const override;

			virtual bool isRead() const override;

			/** Encodes whether all data is available, i.e. referenced image or video data is available. */
			virtual bool isSaved() const override;

			virtual openmittsu::dataproviders::messages::UserMessageState const& getMessageState() const override;

			virtual openmittsu::protocol::MessageTime const& getReceivedAt() const override;
			virtual openmittsu::protocol::MessageTime const& getSeenAt() const override;
			virtual bool isStatusMessage() const override;

			virtual QString const& getCaption() const override;

			virtual openmittsu::protocol::ContactId const& getContactId() const override;
			virtual openmittsu::dataproviders::messages::ContactMessageType const& getMessageType() const override;

			virtual QString const& getContentAsText() const override;
			virtual openmittsu::utility::Location getContentAsLocation() const override;
			virtual MediaFileItem getContentAsImage() const override;
		private:
			openmittsu::protocol::ContactId m_sender;
			openmittsu::protocol::MessageId m_messageId;
			bool m_isMessageFromUs;
			openmittsu::protocol::MessageTime m_createdAt;
			openmittsu::protocol::MessageTime m_sentAt;
			openmittsu::protocol::MessageTime m_modifiedAt;
			bool m_isQueued;
			bool m_isSent;
			QString m_uuid;
			bool m_isRead;
			bool m_isSaved;
			openmittsu::dataproviders::messages::UserMessageState m_messageState;
			openmittsu::protocol::MessageTime m_receivedAt;
			openmittsu::protocol::MessageTime m_seenAt;
			bool m_isStatusMessage;
			QString m_caption;
			openmittsu::dataproviders::messages::ContactMessageType m_contactMessageType;
			QString m_body;
			MediaFileItem m_mediaItem;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEREADONLYCONTACTMESSAGE_H_
