#ifndef OPENMITTSU_DATAPROVIDERS_CONTACTDATAPROVIDER_H_
#define OPENMITTSU_DATAPROVIDERS_CONTACTDATAPROVIDER_H_

#include <QObject>
#include <QString>
#include <QSet>
#include <QHash>

#include <memory>

#include "src/protocol/ContactId.h"
#include "src/protocol/ContactStatus.h"
#include "src/protocol/AccountStatus.h"
#include "src/protocol/ContactIdVerificationStatus.h"
#include "src/protocol/FeatureLevel.h"
#include "src/crypto/PublicKey.h"

#include "src/database/ContactData.h"
#include "src/dataproviders/messages/ContactMessageCursor.h"

namespace openmittsu {
	namespace dataproviders {

		class ContactDataProvider : public QObject {
			Q_OBJECT
		public:
			virtual ~ContactDataProvider() {}

			virtual bool hasContact(openmittsu::protocol::ContactId const& contact) const = 0;

			virtual openmittsu::crypto::PublicKey getPublicKey(openmittsu::protocol::ContactId const& contact) const = 0;
			
			virtual openmittsu::database::ContactData getContactData(openmittsu::protocol::ContactId const& contact) const = 0;

			virtual int getContactCount() const = 0;

			virtual void addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) = 0;
			virtual void addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color) = 0;

			virtual void setFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) = 0;
			virtual void setLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) = 0;
			virtual void setNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) = 0;
			virtual void setAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) = 0;
			virtual void setVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) = 0;
			virtual void setFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) = 0;
			virtual void setColor(openmittsu::protocol::ContactId const& contact, int color) = 0;

			virtual void setAccountStatusBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& status) = 0;
			virtual void setFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels) = 0;

			virtual QSet<openmittsu::protocol::ContactId> getKnownContacts() const = 0;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const = 0;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const = 0;
			virtual QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> getKnownContactsWithPublicKeys() const = 0;
			virtual QHash<openmittsu::protocol::ContactId, QString> getKnownContactsWithNicknames(bool withSelfContactId) const = 0;

			virtual std::shared_ptr<messages::ContactMessageCursor> getContactMessageCursor(openmittsu::protocol::ContactId const& contact) = 0;
		signals:
			void contactChanged(openmittsu::protocol::ContactId const& identity);
			void contactStartedTyping(openmittsu::protocol::ContactId const& identity);
			void contactStoppedTyping(openmittsu::protocol::ContactId const& identity);
			void contactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid);
		};

	}
}

Q_DECLARE_INTERFACE(openmittsu::dataproviders::ContactDataProvider, "ContactDataProvider")

#endif // OPENMITTSU_DATAPROVIDERS_CONTACTDATAPROVIDER_H_
