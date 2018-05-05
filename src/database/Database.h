#ifndef OPENMITTSU_DATABASE_DATABASE_H_
#define OPENMITTSU_DATABASE_DATABASE_H_

#include <QByteArray>
#include <QObject>
#include <QSet>
#include <QString>

#include <memory>

#include "src/database/DatabaseSeekResult.h"
#include "src/dataproviders/SentMessageAcceptor.h"
#include "src/dataproviders/messages/ReadonlyContactMessage.h"
#include "src/dataproviders/messages/ReadonlyGroupMessage.h"
#include "src/crypto/PublicKey.h"
#include "src/protocol/AccountStatus.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/ContactStatus.h"
#include "src/protocol/ContactIdVerificationStatus.h"
#include "src/protocol/FeatureLevel.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/GroupStatus.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

#include "src/utility/Location.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenterWrapper;
		class BackedContact;
		class BackedGroup;
	}

	namespace database {
		class DatabaseReadonlyContactMessage;

		class Database : public QObject {
			Q_OBJECT
		public:
			enum class SortOrder {
				ASCENDING,
				DESCENDING
			};
			enum class SortByMode {
				ColSpecialSortBy,
			};

			virtual ~Database() {}

			// Misc
			virtual void enableTimers() = 0;

			// Information
			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const = 0;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual openmittsu::protocol::ContactId getSelfContact() const = 0;

			virtual bool hasContact(openmittsu::protocol::ContactId const& identity) const = 0;
			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const = 0;
			virtual bool isDeleteted(openmittsu::protocol::GroupId const& group) const = 0;

			// Messages that we want to send
			virtual openmittsu::protocol::MessageId storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) = 0;

			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) = 0;

			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) = 0;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) = 0;

			virtual openmittsu::protocol::MessageId storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) = 0;

			virtual openmittsu::protocol::MessageId storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) = 0;
			virtual openmittsu::protocol::MessageId storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) = 0;

			// Messages that we have received
			// Received messages
			virtual void storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) = 0;
			virtual void storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) = 0;
			virtual void storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) = 0;

			virtual void storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;

			virtual void storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;
			virtual void storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;

			virtual void storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) = 0;
			virtual void storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) = 0;
			virtual void storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) = 0;

			virtual void storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual void storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) = 0;
			virtual void storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) = 0;
			virtual void storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) = 0;
			virtual void storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) = 0;

			// Control
			virtual void storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) = 0;

			virtual void storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) = 0;
			virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) = 0;

			virtual void sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor& messageAcceptor) = 0;

			virtual std::unique_ptr<openmittsu::dataproviders::BackedContact> getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) = 0;
			virtual std::unique_ptr<openmittsu::dataproviders::BackedGroup> getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) = 0;

			// Contact Data
			virtual QString getFirstName(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual QString getLastName(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual QString getNickName(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual openmittsu::protocol::AccountStatus getAccountStatus(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual openmittsu::protocol::ContactIdVerificationStatus getVerificationStatus(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual openmittsu::protocol::FeatureLevel getFeatureLevel(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual int getColor(openmittsu::protocol::ContactId const& contact) const = 0;
			virtual int getContactCount() const = 0;
			virtual int getContactMessageCount(openmittsu::protocol::ContactId const& contact) const = 0;

			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::ContactId const& contact, std::size_t n) const = 0;
			virtual std::unique_ptr<DatabaseReadonlyContactMessage> getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid) const = 0;

			virtual void setFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) = 0;
			virtual void setLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) = 0;
			virtual void setNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) = 0;
			virtual void setAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) = 0;
			virtual void setVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) = 0;
			virtual void setFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) = 0;
			virtual void setColor(openmittsu::protocol::ContactId const& contact, int color) = 0;

			// Group Data
			virtual QString getGroupTitle(openmittsu::protocol::GroupId const& group) const = 0;
			virtual QString getGroupDescription(openmittsu::protocol::GroupId const& group) const = 0;
			virtual bool getGroupHasImage(openmittsu::protocol::GroupId const& group) const = 0;
			virtual openmittsu::database::MediaFileItem getGroupImage(openmittsu::protocol::GroupId const& group) const = 0;
			virtual bool getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const = 0;
			virtual int getGroupCount() const = 0;
			virtual int getGroupMessageCount(openmittsu::protocol::GroupId const& group) const = 0;
			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const = 0;

			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::GroupId const& group, std::size_t n) const = 0;
			virtual std::unique_ptr<DatabaseReadonlyGroupMessage> getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid) const = 0;

			// Seeking, Searching
			virtual DatabaseSeekResult seekNextMessage(openmittsu::protocol::ContactId const& identity, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const = 0;
			virtual DatabaseSeekResult seekFirstOrLastMessage(openmittsu::protocol::ContactId const& identity, bool first, SortByMode sortByMode) const = 0;
			virtual DatabaseSeekResult seekNextMessage(openmittsu::protocol::GroupId const& group, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const = 0;
			virtual DatabaseSeekResult seekFirstOrLastMessage(openmittsu::protocol::GroupId const& group, bool first, SortByMode sortByMode) const = 0;

			virtual void setGroupTitle(openmittsu::protocol::GroupId const& group, QString const& newTitle) = 0;
			virtual void setGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& newImage) = 0;
			virtual void setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) = 0;
		signals:
			void contactChanged(openmittsu::protocol::ContactId const& identity);
			void groupChanged(openmittsu::protocol::GroupId const& changedGroupId);
			void contactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid);
			void groupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
			void receivedNewContactMessage(openmittsu::protocol::ContactId const& identity);
			void receivedNewGroupMessage(openmittsu::protocol::GroupId const& group);
			void messageChanged(QString const& uuid);
			void haveQueuedMessages();
			void contactStartedTyping(openmittsu::protocol::ContactId const& identity);
			void contactStoppedTyping(openmittsu::protocol::ContactId const& identity);
		};
	}
}

Q_DECLARE_INTERFACE(openmittsu::database::Database, "openmittsu.database.Database")

#endif // OPENMITTSU_DATABASE_DATABASE_H_
