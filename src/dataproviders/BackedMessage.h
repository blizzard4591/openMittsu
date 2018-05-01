#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_

#include <QString>
#include <QObject>
#include <QMutex>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/messages/ReadonlyUserMessage.h"
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

			messages::UserMessageState const& getMessageState() const;

			openmittsu::protocol::MessageTime const& getCreatedAt() const;
			openmittsu::protocol::MessageTime const& getSentAt() const;
			openmittsu::protocol::MessageTime const& getReceivedAt() const;
			openmittsu::protocol::MessageTime const& getSeenAt() const;
			openmittsu::protocol::MessageTime const& getModifiedAt() const;
			bool isStatusMessage() const;

			openmittsu::protocol::ContactId const& getSender() const;
			BackedContact const& getContact() const;

			bool isMessageFromUs() const;

			QString getContentAsText() const;
			openmittsu::utility::Location getContentAsLocation() const;
			openmittsu::database::MediaFileItem getContentAsImage() const;

			QString const& getCaption() const;

			virtual void setIsSeen() = 0;

			bool operator <(BackedMessage const& other) const;
			bool operator <=(BackedMessage const& other) const;
		public slots:
			void onMessageChanged(QString const& uuid);
		signals:
			void messageDataChanged();
		protected:
			virtual void loadCache() = 0;
			virtual messages::ReadonlyUserMessage const& getMessage() const = 0;
			
			QString const m_uuid;
			BackedContact const m_contact;
			bool const m_isMessageFromUs;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDMESSAGE_H_
