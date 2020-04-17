#ifndef OPENMITTSU_DATABASE_DATABASEWRAPPER_H_
#define OPENMITTSU_DATABASE_DATABASEWRAPPER_H_

#include <memory>

#include "src/database/Database.h"
#include "src/database/DatabasePointerAuthority.h"

namespace openmittsu {
	namespace database {

		class DatabaseWrapper : public Database {
			Q_OBJECT
		public:
			DatabaseWrapper(DatabasePointerAuthority const* databasePointerAuthority);
			DatabaseWrapper(DatabaseWrapper const& other);
			virtual ~DatabaseWrapper();

			bool hasDatabase() const;
		signals:
			void gotDatabase();
		private slots:
			void onDatabasePointerAuthorityHasNewDatabase();

			void onDatabaseContactChanged(openmittsu::protocol::ContactId const& identity);
			void onDatabaseGroupChanged(openmittsu::protocol::GroupId const& changedGroupId);
			void onDatabaseContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid);
			void onDatabaseGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
			void onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const& identity);
			void onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const& group);
			void onDatabaseMessageChanged(QString const& uuid);
			void onDatabaseMessageDeleted(QString const& uuid);
			void onDatabaseHaveQueuedMessages();
			void onDatabaseContactStartedTyping(openmittsu::protocol::ContactId const& identity);
			void onDatabaseContactStoppedTyping(openmittsu::protocol::ContactId const& identity);
			void onDatabaseOptionsChanged();
		protected:
			DatabasePointerAuthority const* m_databasePointerAuthority;
			std::weak_ptr<Database> m_database;
			Qt::ConnectionType m_connectionType;
		public:
			// Inherited via Database
			
			// Misc
			virtual void enableTimers() override;
			// Information
			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const override;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::ContactId getSelfContact() const override;
			virtual bool hasContact(openmittsu::protocol::ContactId const& identity) const override;
			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const override;
			virtual bool isDeleteted(openmittsu::protocol::GroupId const& group) const override;
			// Messages that we want to send
			virtual openmittsu::protocol::MessageId storeSentContactMessageAudio(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageFile(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageVideo(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageAudio(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageFile(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual openmittsu::protocol::MessageId storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) override;
			// Messages that we have received
			// Received messages
			virtual void storeReceivedContactMessageAudio(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void storeReceivedContactMessageFile(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) override;
			virtual void storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;
			virtual void storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void storeReceivedContactMessageVideo(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual void storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void storeReceivedGroupMessageAudio(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void storeReceivedGroupMessageFile(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) override;
			virtual void storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;
			virtual void storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void storeReceivedGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual void storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual void storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) override;
			virtual void storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) override;
			virtual void storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;
			virtual void storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;
			// Control
			virtual void storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) override;
			virtual void storeNewContact(QVector<NewContactData> const& newContactData) override;
			virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) override;
			virtual void storeNewGroup(QVector<NewGroupData> const& newGroupData) override;
			virtual void sendAllWaitingMessages(std::shared_ptr<openmittsu::dataproviders::SentMessageAcceptor> messageAcceptor) override;
			// Contact Data
			virtual ContactData getContactData(openmittsu::protocol::ContactId const& contact, bool fetchMessageCount) const override;
			virtual ContactToContactDataMap getContactDataAll(bool fetchMessageCount) const override;
			virtual openmittsu::crypto::PublicKey getContactPublicKey(openmittsu::protocol::ContactId const& identity) const override;
			virtual int getContactCount() const override;
			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::ContactId const& contact, std::size_t n) override;
			virtual std::shared_ptr<DatabaseReadonlyContactMessage> getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid) override;
			virtual void setContactFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) override;
			virtual void setContactLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) override;
			virtual void setContactNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) override;
			virtual void setContactAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) override;
			virtual void setContactVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) override;
			virtual void setContactFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) override;
			virtual void setContactColor(openmittsu::protocol::ContactId const& contact, int color) override;
			// Group Data
			virtual GroupData getGroupData(openmittsu::protocol::GroupId const& group, bool withDescription) const override;
			virtual GroupToGroupDataMap getGroupDataAll(bool withDescription) const override;
			virtual int getGroupCount() const override;
			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const override;
			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::GroupId const& group, std::size_t n) override;
			virtual std::shared_ptr<DatabaseReadonlyGroupMessage> getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid) override;
			// Mass Data, checks
			virtual std::shared_ptr<openmittsu::backup::IdentityBackup> getBackup() const override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const override;
			virtual void setContactAccountStatusBatch(ContactToAccountStatusMap const& status) override;
			virtual void setContactFeatureLevelBatch(ContactToFeatureLevelMap const& featureLevels) override;
			virtual GroupToTitleMap getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const override;
			virtual openmittsu::protocol::MessageTime getGroupLastSyncRequestTime(openmittsu::protocol::GroupId const& group) override;
			// Deleting Messages
			virtual void deleteContactMessageByUuid(openmittsu::protocol::ContactId const& contact, QString const& uuid) override;
			virtual void deleteContactMessagesByAge(openmittsu::protocol::ContactId const& contact, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) override;
			virtual void deleteContactMessagesByCount(openmittsu::protocol::ContactId const& contact, bool oldestOrNewest, int count) override;
			virtual void deleteGroupMessageByUuid(openmittsu::protocol::GroupId const& group, QString const& uuid) override;
			virtual void deleteGroupMessagesByAge(openmittsu::protocol::GroupId const& group, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) override;
			virtual void deleteGroupMessagesByCount(openmittsu::protocol::GroupId const& group, bool oldestOrNewest, int count) override;
			// Options
			virtual openmittsu::database::OptionNameToValueMap getOptions() override;
			virtual void setOptions(openmittsu::database::OptionNameToValueMap const& options) override;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEWRAPPER_H_
