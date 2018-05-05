#ifndef OPENMITTSU_DATABASE_SIMPLEDATABASE_H_
#define OPENMITTSU_DATABASE_SIMPLEDATABASE_H_

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>

#include "src/protocol/ContactId.h"
#include "src/protocol/AccountStatus.h"
#include "src/protocol/ContactIdVerificationStatus.h"
#include "src/protocol/FeatureLevel.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/backup/IdentityBackup.h"
#include "src/crypto/PublicKey.h"
#include "src/crypto/KeyPair.h"

#include "src/database/internal/DatabaseContactAndGroupDataProvider.h"
#include "src/database/internal/DatabaseMessageCursor.h"
#include "src/database/internal/DatabaseContactMessage.h"
#include "src/database/internal/DatabaseContactMessageCursor.h"
#include "src/database/internal/DatabaseControlMessage.h"
#include "src/database/internal/DatabaseGroupMessage.h"
#include "src/database/internal/DatabaseGroupMessageCursor.h"
#include "src/database/internal/DatabaseMessage.h"
#include "src/database/internal/ExternalMediaFileStorage.h"
#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/database/DatabaseReadonlyContactMessage.h"
#include "src/dataproviders/messages/ContactMessageType.h"
#include "src/dataproviders/messages/ControlMessageType.h"
#include "src/dataproviders/messages/GroupMessageType.h"

#include "src/utility/Location.h"
#include "src/dataproviders/SentMessageAcceptor.h"
#include "src/database/Database.h"
#include "src/dataproviders/ReceivedMessageAcceptor.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedGroup.h"

namespace openmittsu {
	namespace backup {
		class ContactMessageBackupObject;
		class GroupMessageBackupObject;
		class ContactMediaItemBackupObject;
		class GroupMediaItemBackupObject;
	}

	namespace database {

		class SimpleDatabase : public openmittsu::database::Database, public internal::InternalDatabaseInterface {
			Q_OBJECT
			Q_INTERFACES(openmittsu::database::Database)
		public:
			explicit SimpleDatabase(QString const& filename, QString const& password, QDir const& mediaStorageLocation);
			explicit SimpleDatabase(QString const& filename, openmittsu::protocol::ContactId const& selfContact, openmittsu::crypto::KeyPair const& selfLongTermKeyPair, QString const& password, QDir const& mediaStorageLocation);
			virtual ~SimpleDatabase();

			static QString getDefaultDatabaseFileName();

			// Backup access
			void storeContactMessagesFromBackup(QList<openmittsu::backup::ContactMessageBackupObject> const& messages);
			void storeGroupMessagesFromBackup(QList<openmittsu::backup::GroupMessageBackupObject> const& messages);
			void storeContactMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items);
			void storeGroupMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items);

			virtual bool hasContact(openmittsu::protocol::ContactId const& identity) const override;
			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const override;
			virtual bool isDeleteted(openmittsu::protocol::GroupId const& group) const override;

			openmittsu::backup::IdentityBackup getBackup();

			bool hasOption(QString const& optionName);
			QString getOptionValueAsString(QString const& optionName);
			bool getOptionValueAsBool(QString const& optionName);
			QByteArray getOptionValueAsByteArray(QString const& optionName);

			void setOptionValue(QString const& optionName, QString const& optionValue);
			void setOptionValue(QString const& optionName, bool const& optionValue);
			void setOptionValue(QString const& optionName, QByteArray const& optionValue);

			virtual std::unique_ptr<openmittsu::dataproviders::BackedContact> getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) override;
			virtual std::unique_ptr<openmittsu::dataproviders::BackedGroup> getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) override;

			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const override;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::ContactId getSelfContact() const override;

			/** AHHH FUCK THIS **/
			virtual std::unique_ptr<DatabaseReadonlyContactMessage> getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid) const override;
			virtual std::unique_ptr<DatabaseReadonlyGroupMessage> getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid) const override;


			openmittsu::crypto::PublicKey getContactPublicKey(openmittsu::protocol::ContactId const& identity) const;
			QString getContactNickname(openmittsu::protocol::ContactId const& identity) const;
			openmittsu::protocol::AccountStatus getContactAccountStatus(openmittsu::protocol::ContactId const& identity) const;
			openmittsu::protocol::ContactIdVerificationStatus getContactVerficationStatus(openmittsu::protocol::ContactId const& identity) const;
			openmittsu::protocol::FeatureLevel getContactFeatureLevel(openmittsu::protocol::ContactId const& identity) const;

			QSet<openmittsu::protocol::ContactId> getKnownContacts() const;
			QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> getKnownContactsWithPublicKeys() const;
			QHash<openmittsu::protocol::ContactId, QString> getKnownContactsWithNicknames(bool withSelfContactId = true) const;

			void setContactNickname(openmittsu::protocol::ContactId const& identity, QString const& nickname);
			void setContactAccountStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::AccountStatus const& status);
			void setContactVerficationStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus);
			void setContactFeatureLevel(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::FeatureLevel const& featureLevel);

			void setContactAccountStatusBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& status);
			void setContactFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels);

			QSet<openmittsu::protocol::GroupId> getKnownGroups() const;
			QSet<openmittsu::protocol::ContactId> getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const;
			QSet<openmittsu::protocol::ContactId> getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const;

			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const override;
			QString getGroupTitle(openmittsu::protocol::GroupId const& group) const;
			QString getGroupDescription(openmittsu::protocol::GroupId const& group) const;
			MediaFileItem getGroupImage(openmittsu::protocol::GroupId const& group) const;
			bool getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const;

			QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> getKnownGroupsWithMembersAndTitles() const;
			QSet<openmittsu::protocol::GroupId> getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const;

			int getContactCount() const;
			int getGroupCount() const;
			int getContactMessageCount() const;
			int getGroupMessageCount() const;
			int getMediaItemCount() const;

			internal::DatabaseContactMessageCursor getMessageCursor(openmittsu::protocol::ContactId const& contact);
			internal::DatabaseGroupMessageCursor getMessageCursor(openmittsu::protocol::GroupId const& group);

			openmittsu::protocol::MessageId getNextMessageId(openmittsu::protocol::ContactId const& contact);
			openmittsu::protocol::MessageId getNextMessageId(openmittsu::protocol::GroupId const& group);

			// Queries
			virtual QSqlQuery getQueryObject() const override;
			virtual bool transactionStart() override;
			virtual bool transactionCommit() override;

			// Seeking
			virtual DatabaseSeekResult seekNextMessage(openmittsu::protocol::ContactId const& identity, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const override;
			virtual DatabaseSeekResult seekFirstOrLastMessage(openmittsu::protocol::ContactId const& identity, bool first, SortByMode sortByMode) const override;
			virtual DatabaseSeekResult seekNextMessage(openmittsu::protocol::GroupId const& group, QString const& uuid, SortOrder sortOrder, SortByMode sortByMode) const override;
			virtual DatabaseSeekResult seekFirstOrLastMessage(openmittsu::protocol::GroupId const& group, bool first, SortByMode sortByMode) const override;

			// Media Files
			virtual MediaFileItem getMediaItem(QString const& uuid) const override;
			virtual QString insertMediaItem(QByteArray const& data) override;
			virtual void removeMediaItem(QString const& uuid) override;

			friend class internal::DatabaseMessage;
			friend class internal::DatabaseContactMessage;
			friend class internal::DatabaseControlMessage;
			friend class internal::DatabaseGroupMessage;
			friend class internal::DatabaseContactMessageCursor;
			friend class internal::DatabaseGroupMessageCursor;
			friend class internal::DatabaseMessageCursor;
			friend class internal::ExternalMediaFileStorage;
			friend class internal::DatabaseContactAndGroupDataProvider;
		public slots:
			virtual void enableTimers() override;
			virtual void sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor& messageAcceptor) override;

			virtual openmittsu::protocol::MessageId storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) override;

			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessageId) override;

			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;

			virtual openmittsu::protocol::MessageId storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) override;

			virtual openmittsu::protocol::MessageId storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) override;

			// Received messages
			virtual void storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) override;
			virtual void storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;

			virtual void storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;

			virtual void storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;

			virtual void storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) override;
			virtual void storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;

			virtual void storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual void storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) override;
			virtual void storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) override;
			virtual void storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;
			virtual void storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;

			virtual void storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;
			virtual void storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;

			// New Contact, new Group, group changes...
			virtual void storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) override;
			virtual void storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color);

			virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) override;
			virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync);

			void announceMessageChanged(QString const& uuid);
			void announceContactChanged(openmittsu::protocol::ContactId const& contact);
			void announceGroupChanged(openmittsu::protocol::GroupId const& group);
			void announceNewMessage(openmittsu::protocol::ContactId const& contact, QString const& messageUuid);
			void announceNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
			void announceReceivedNewMessage(openmittsu::protocol::ContactId const& contact);
			void announceReceivedNewMessage(openmittsu::protocol::GroupId const& group);
		private:
			QSqlDatabase database;
			QString const m_driverNameCrypto;
			QString const m_driverNameStandard;
			QString const m_connectionName;
			QString const m_password;

			bool m_usingCryptoDb;

			openmittsu::protocol::ContactId m_selfContact;
			openmittsu::crypto::KeyPair m_selfLongTermKeyPair;
			std::unique_ptr<openmittsu::backup::IdentityBackup> m_identityBackup;

			internal::DatabaseContactAndGroupDataProvider m_contactAndGroupDataProvider;
			internal::ExternalMediaFileStorage m_mediaFileStorage;

			QTimer queueTimeoutTimer;

			enum class Tables {
				Contacts,
				ContactMessages,
				ControlMessages,
				FeatureLevels,
				Groups,
				GroupMessages,
				Media,
				Settings,
				TableVersions,
				SqliteMaster,
			};

			bool doesTableExist(Tables const& table);
			int getTableVersion(Tables const& table);
			QString getTableName(Tables const& table);
			QString generateUuid() const;
			QString getCreateStatementForTable(Tables const& table);
			int createTableIfMissingAndGetVersion(Tables const& table, int createStatementVersion);
			void setTableVersion(Tables const& table, int tableVersion);
			void createOrUpdateTables();
			QString getOptionValueInternal(QString const& optionName, bool isInternalOption = false);
			bool hasOptionInternal(QString const& optionName, bool isInternalOption = false);
			void setOptionInternal(QString const& optionName, QString const& optionValue, bool isInternalOption = false);
			void setBackup(openmittsu::protocol::ContactId selfId, openmittsu::crypto::KeyPair key);
			void setupQueueTimer();
			void setKey(QString const& password);
			void updateCachedIdentityBackup();
		private slots:
			void onQueueTimeoutTimerFire();
		};

	}
}

#endif // OPENMITTSU_DATABASE_SIMPLEDATABASE_H_
