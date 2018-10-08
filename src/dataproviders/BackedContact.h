#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_

#include <QString>
#include <QObject>
#include <QVector>

#include "src/crypto/PublicKey.h"

#include "src/dataproviders/MessageSource.h"

#include "src/database/DatabaseWrapper.h"
#include "src/database/DatabaseReadonlyContactMessage.h"
#include "src/dataproviders/BackedContactAndGroupPool.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"
#include "src/dataproviders/MessageCenterWrapper.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/AccountStatus.h"
#include "src/utility/DeferredConstructionWrapper.h"
#include "src/utility/Location.h"

namespace openmittsu {
	namespace database {
		class DatabaseWrapper;
	}

	namespace dataproviders {
		class BackedContactMessage;
		class BackedGroupMessage;

		class BackedContact : public QObject, public MessageSource {
			Q_OBJECT
		public:
			BackedContact(openmittsu::protocol::ContactId const& contactId, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter, BackedContactAndGroupPool& pool);
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

			friend class BackedContactMessage;
			friend class BackedGroupMessage;
		protected:
			openmittsu::database::DatabaseReadonlyContactMessage fetchMessageByUuid(QString const& uuid);
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
			openmittsu::database::DatabaseWrapper m_database;
			openmittsu::dataproviders::MessageCenterWrapper m_messageCenter;
			BackedContactAndGroupPool& m_pool;

			openmittsu::database::ContactData m_contactData;
		private slots:
			void slotIdentityChanged(openmittsu::protocol::ContactId const& changedContactId);
			void slotNewMessage(openmittsu::protocol::ContactId const& contactId, QString const& messageUuid);
			void slotContactStartedTyping(openmittsu::protocol::ContactId const& contactId);
			void slotContactStoppedTyping(openmittsu::protocol::ContactId const& contactId);
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDCONTACT_H_
