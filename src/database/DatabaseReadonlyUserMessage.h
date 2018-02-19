#ifndef OPENMITTSU_DATABASE_DATABASEREADONLYUSERMESSAGE_H_
#define OPENMITTSU_DATABASE_DATABASEREADONLYUSERMESSAGE_H_

#include <QString>
#include <QSqlQuery>

#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/database/DatabaseReadonlyMessage.h"
#include "src/dataproviders/messages/ReadonlyUserMessage.h"

namespace openmittsu {
	namespace database {
		class DatabaseReadonlyUserMessage : public virtual DatabaseReadonlyMessage, public virtual openmittsu::dataproviders::messages::ReadonlyUserMessage {
		public:
			//explicit DatabaseReadonlyUserMessage(DatabaseWrapper const& database, openmittsu::protocol::MessageId const& messageId);
			explicit DatabaseReadonlyUserMessage(openmittsu::protocol::ContactId sender, openmittsu::protocol::MessageId messageId, bool isMessageFromUs, bool isOutbox, openmittsu::protocol::MessageTime createdAt, openmittsu::protocol::MessageTime sentAt, openmittsu::protocol::MessageTime modifiedAt, bool isQueued, bool isSent, QString uuid, bool isRead, bool isSaved, openmittsu::dataproviders::messages::UserMessageState messageState, openmittsu::protocol::MessageTime receivedAt, openmittsu::protocol::MessageTime seenAt, bool isStatusMessage, QString caption);
			virtual ~DatabaseReadonlyUserMessage();

			virtual bool isRead() const override;

			/** Encodes whether all data is available, i.e. referenced image or video data is available. */
			virtual bool isSaved() const override;

			virtual openmittsu::dataproviders::messages::UserMessageState getMessageState() const override;
			
			virtual openmittsu::protocol::MessageTime getReceivedAt() const override;
			virtual openmittsu::protocol::MessageTime getSeenAt() const override;
			virtual bool isStatusMessage() const override;

			virtual QString getCaption() const override;
		protected:
			bool m_isRead;
			bool m_isSaved;
			openmittsu::dataproviders::messages::UserMessageState m_messageState;
			openmittsu::protocol::MessageTime m_receivedAt;
			openmittsu::protocol::MessageTime m_seenAt;
			bool m_isStatusMessage;
			QString m_caption;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEREADONLYUSERMESSAGE_H_
