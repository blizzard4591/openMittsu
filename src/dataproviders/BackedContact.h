#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_

#include <QString>
#include <QObject>
#include <QVector>

#include "src/crypto/PublicKey.h"

#include "src/dataproviders/MessageSource.h"

#include "src/utility/Location.h"
#include "src/database/Database.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"
#include "src/dataproviders/MessageCenterWrapper.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/AccountStatus.h"

namespace openmittsu {
	namespace dataproviders {
		class BackedContactMessage;

		class BackedContact : public QObject, public MessageSource {
			Q_OBJECT
		public:
			BackedContact(openmittsu::protocol::ContactId const& contactId, openmittsu::crypto::PublicKey const& contactPublicKey, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);
			BackedContact(BackedContact const& other);
			virtual ~BackedContact();

			QString getFirstName() const;
			QString getLastName() const;
			QString getName() const;

			QString getNickname() const;
			openmittsu::crypto::PublicKey const& getPublicKey() const;
			openmittsu::protocol::ContactId const& getId() const;
			openmittsu::protocol::AccountStatus getActivityStatus() const;

			virtual int getMessageCount() const override;

			virtual QVector<QString> getLastMessageUuids(std::size_t n) override;
			BackedContactMessage getMessageByUuid(QString const& uuid);

			void setNickname(QString const& newNickname);
			void setFirstName(QString const& newFirstName);
			void setLastName(QString const& newLastName);
		public slots:
			bool sendTextMessage(QString const& text);
			bool sendImageMessage(QByteArray const& image, QString const& caption);
			bool sendLocationMessage(openmittsu::utility::Location const& location);

			void sendTypingNotification(bool typingStopped);
		signals:
			void contactDataChanged();
			void contactStartedTyping();
			void contactStoppedTyping();
			void newMessageAvailable(QString const& uuid);
		private:
			openmittsu::protocol::ContactId const m_contactId;
			openmittsu::crypto::PublicKey const m_contactPublicKey;
			openmittsu::database::DatabaseWrapper m_database;
			openmittsu::dataproviders::MessageCenterWrapper m_messageCenter;

			std::shared_ptr<messages::ContactMessageCursor> m_cursor;
		private slots:
			void slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId);
			void slotNewMessage(openmittsu::protocol::ContactId const& contactId, QString const& messageUuid);
			void slotContactStartedTyping(openmittsu::protocol::ContactId const& contactId);
			void slotContactStoppedTyping(openmittsu::protocol::ContactId const& contactId);
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_
