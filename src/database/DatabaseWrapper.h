#ifndef OPENMITTSU_DATABASE_DATABASEWRAPPER_H_
#define OPENMITTSU_DATABASE_DATABASEWRAPPER_H_

#include <memory>

#include "src/database/Database.h"
#include "src/database/DatabasePointerAuthority.h"

namespace openmittsu {
	namespace database {

		class DatabaseWrapper : public Database {
		public:
			DatabaseWrapper(DatabasePointerAuthority const& databasePointerAuthority);
			virtual ~DatabaseWrapper();

		private slots:
			void onDatabasePointerAuthorityHasNewDatabase();
		private:
			DatabasePointerAuthority const& m_databasePointerAuthority;
			std::weak_ptr<Database> m_database;

		public:
			void setupConnection();

			// Inherited via Database
			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const & group) const override;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const & contact) const override;
			virtual openmittsu::protocol::ContactId const & getSelfContact() const override;
			virtual bool hasContact(openmittsu::protocol::ContactId const & identity) const override;
			virtual bool hasGroup(openmittsu::protocol::GroupId const & group) const override;
			virtual bool isDeleteted(openmittsu::protocol::GroupId const & group) const override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageText(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & message) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageImage(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, QString const & caption) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageLocation(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::utility::Location const & location) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageText(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & message) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, QString const & caption) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageLocation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, openmittsu::utility::Location const & location) override;
			virtual openmittsu::protocol::MessageId storeSentGroupCreation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const & members, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QByteArray const & image, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetTitle(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, QString const & groupTitle, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSyncRequest(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupLeave(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageTime const & timeCreated, bool isQueued, bool apply) override;
			virtual void storeReceivedContactMessageText(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & message) override;
			virtual void storeReceivedContactMessageImage(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image, QString const & caption) override;
			virtual void storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, openmittsu::utility::Location const & location) override;
			virtual void storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageId const & referredMessageId) override;
			virtual void storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent) override;
			virtual void storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent) override;
			virtual void storeReceivedGroupMessageText(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & message) override;
			virtual void storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image, QString const & caption) override;
			virtual void storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, openmittsu::utility::Location const & location) override;
			virtual void storeReceivedGroupCreation(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QSet<openmittsu::protocol::ContactId> const & members) override;
			virtual void storeReceivedGroupSetImage(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QByteArray const & image) override;
			virtual void storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived, QString const & groupTitle) override;
			virtual void storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived) override;
			virtual void storeReceivedGroupLeave(openmittsu::protocol::GroupId const & group, openmittsu::protocol::ContactId const & sender, openmittsu::protocol::MessageId const & messageId, openmittsu::protocol::MessageTime const & timeSent, openmittsu::protocol::MessageTime const & timeReceived) override;
			virtual void storeMessageSendFailed(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageId const & messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::ContactId const & receiver, openmittsu::protocol::MessageId const & messageId) override;
			virtual void storeMessageSendFailed(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageId const & messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::GroupId const & group, openmittsu::protocol::MessageId const & messageId) override;
			virtual void storeNewContact(openmittsu::protocol::ContactId const & contact, openmittsu::crypto::PublicKey const & publicKey) override;
			virtual void storeNewGroup(openmittsu::protocol::GroupId const & groupId, QSet<openmittsu::protocol::ContactId> const & members, bool isAwaitingSync) override;
			virtual void sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor & messageAcceptor) override;
			virtual std::unique_ptr<openmittsu::dataproviders::BackedContact> getBackedContact(openmittsu::protocol::ContactId const & contact, openmittsu::dataproviders::MessageCenter & messageCenter) override;
			virtual std::unique_ptr<openmittsu::dataproviders::BackedGroup> getBackedGroup(openmittsu::protocol::GroupId const & group, openmittsu::dataproviders::MessageCenter & messageCenter) override;
			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const & group, bool excludeSelfContact) const override;
			virtual void enableTimers() override;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEWRAPPER_H_
