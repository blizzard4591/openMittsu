#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_

#include <QString>
#include <QObject>
#include <QMutex>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/messages/UserMessage.h"
#include "src/dataproviders/messages/UserMessageState.h"
#include "src/utility/Location.h"

namespace openmittsu {
	namespace dataproviders {

		class BackedMessage : public QObject {
			Q_OBJECT
		public:
			BackedMessage(QString const& uuid, BackedContact const& sender, bool isMessageFromUs, openmittsu::protocol::MessageId const& messageId);
			BackedMessage(BackedMessage const& other);
			virtual ~BackedMessage();

			bool isRead() const;
			bool isSent() const;

			openmittsu::protocol::MessageId const& getMessageId() const;

			messages::UserMessageState getMessageState() const;

			openmittsu::protocol::MessageTime getCreatedAt() const;
			openmittsu::protocol::MessageTime getSentAt() const;
			openmittsu::protocol::MessageTime getReceivedAt() const;
			openmittsu::protocol::MessageTime getSeenAt() const;
			openmittsu::protocol::MessageTime getModifiedAt() const;
			bool isStatusMessage() const;

			openmittsu::protocol::ContactId const& getSender() const;
			BackedContact const& getContact() const;

			bool isMessageFromUs() const;

			QString getContentAsText() const;
			openmittsu::utility::Location getContentAsLocation() const;
			QByteArray getContentAsImage() const;

			QString getCaption() const;

			virtual void setIsSeen() = 0;

			bool operator <(BackedMessage const& other) const;
			bool operator <=(BackedMessage const& other) const;
		public slots:
			void onMessageChanged(QString const& uuid);
		signals:
			void messageDataChanged();
		protected:
			void loadCache();
			virtual messages::UserMessage const& getMessage() const = 0;
		private:
			mutable QMutex m_mutex;
			QString const m_uuid;
			BackedContact const m_contact;
			openmittsu::protocol::ContactId const m_sender;
			bool const m_isMessageFromUs;
			openmittsu::protocol::MessageId const m_messageId;

			bool m_cacheLoaded;

			bool m_isRead;
			bool m_isSent;
			messages::UserMessageState m_messageState;
			openmittsu::protocol::MessageTime m_timeOrderBy;
			openmittsu::protocol::MessageTime m_timeCreatedAt;
			openmittsu::protocol::MessageTime m_timeSent;
			openmittsu::protocol::MessageTime m_timeReceived;
			openmittsu::protocol::MessageTime m_timeSeen;
			openmittsu::protocol::MessageTime m_timeModified;
			bool m_isStatusMessage;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_
