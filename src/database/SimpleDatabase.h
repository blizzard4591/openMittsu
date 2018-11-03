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

			bool hasOption(QString const& optionName);
			QString getOptionValueAsString(QString const& optionName);
			bool getOptionValueAsBool(QString const& optionName);
			QByteArray getOptionValueAsByteArray(QString const& optionName);

			void setOptionValue(QString const& optionName, QString const& optionValue);
			void setOptionValue(QString const& optionName, bool const& optionValue);
			void setOptionValue(QString const& optionName, QByteArray const& optionValue);

			QSet<openmittsu::protocol::ContactId> getKnownContacts() const;
			QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> getKnownContactsWithPublicKeys() const;
			//virtual QHash<openmittsu::protocol::ContactId, QString> getKnownContactsWithNicknames(bool withSelfContactId = true) const override;

			QSet<openmittsu::protocol::GroupId> getKnownGroups() const;
			

			QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> getKnownGroupsWithMembersAndTitles() const;
			

			virtual int getContactCount() const override;
			virtual int getGroupCount() const override;
			int getContactMessageCount() const;
			int getGroupMessageCount() const;
			int getMediaItemCount() const;

			internal::DatabaseContactMessageCursor getMessageCursor(openmittsu::protocol::ContactId const& contact);
			internal::DatabaseGroupMessageCursor getMessageCursor(openmittsu::protocol::GroupId const& group);

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
			virtual bool hasContact(openmittsu::protocol::ContactId const& identity) const override;
			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const override;
			virtual bool isDeleteted(openmittsu::protocol::GroupId const& group) const override;

			virtual OptionNameToValueMap getOptions() override;
			virtual void setOptions(OptionNameToValueMap const& options) override;

			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const override;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::ContactId getSelfContact() const override;

			virtual std::shared_ptr<DatabaseReadonlyContactMessage> getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid) override;
			virtual std::shared_ptr<DatabaseReadonlyGroupMessage> getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid) override;

			virtual ContactData getContactData(openmittsu::protocol::ContactId const& contact, bool fetchMessageCount) const override;
			virtual ContactToContactDataMap getContactDataAll(bool fetchMessageCount) const override;
			virtual GroupData getGroupData(openmittsu::protocol::GroupId const& group, bool withDescription) const override;
			virtual GroupToGroupDataMap getGroupDataAll(bool withDescription) const override;

			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::ContactId const& contact, std::size_t n) override;
			virtual QVector<QString> getLastMessageUuids(openmittsu::protocol::GroupId const& group, std::size_t n) override;

			virtual openmittsu::crypto::PublicKey getContactPublicKey(openmittsu::protocol::ContactId const& identity) const override;


			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const override;

			virtual void setContactFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) override;
			virtual void setContactLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) override;
			virtual void setContactNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) override;
			virtual void setContactAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) override;
			virtual void setContactVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) override;
			virtual void setContactFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) override;
			virtual void setContactColor(openmittsu::protocol::ContactId const& contact, int color) override;

			virtual void setContactAccountStatusBatch(ContactToAccountStatusMap const& status) override;
			virtual void setContactFeatureLevelBatch(ContactToFeatureLevelMap const& featureLevels) override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const override;
			virtual GroupToTitleMap getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const override;

			virtual std::shared_ptr<openmittsu::backup::IdentityBackup> getBackup() const override;

			virtual void enableTimers() override;
			virtual void sendAllWaitingMessages(std::shared_ptr<openmittsu::dataproviders::SentMessageAcceptor> messageAcceptor) override;

			virtual openmittsu::protocol::MessageId storeSentContactMessageAudio(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& audio, quint16 lengthInSeconds) override;
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
			virtual openmittsu::protocol::MessageId storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) override;
			virtual openmittsu::protocol::MessageId storeSentGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;

			virtual openmittsu::protocol::MessageId storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) override;
			virtual openmittsu::protocol::MessageId storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) override;
			virtual openmittsu::protocol::MessageId storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) override;

			// Received messages
			virtual void storeReceivedContactMessageAudio(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& audio, quint16 lengthInSeconds) override;
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
			virtual void storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) override;
			virtual void storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;
			virtual void storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void storeReceivedGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;

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
			virtual void storeNewContact(QVector<NewContactData> const& newContactData) override;
			//virtual void storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color);

			virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) override;
			//virtual void storeNewGroup(openmittsu::protocol::GroupId const& groupId, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync);
			virtual void storeNewGroup(QVector<NewGroupData> const& newGroupData) override;

			// Deleting Messages
			virtual void deleteContactMessageByUuid(openmittsu::protocol::ContactId const& contact, QString const& uuid) override;
			virtual void deleteContactMessagesByAge(openmittsu::protocol::ContactId const& contact, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) override;
			virtual void deleteContactMessagesByCount(openmittsu::protocol::ContactId const& contact, bool oldestOrNewest, int count) override;
			virtual void deleteGroupMessageByUuid(openmittsu::protocol::GroupId const& group, QString const& uuid) override;
			virtual void deleteGroupMessagesByAge(openmittsu::protocol::GroupId const& group, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) override;
			virtual void deleteGroupMessagesByCount(openmittsu::protocol::GroupId const& group, bool oldestOrNewest, int count) override;

			virtual void announceMessageChanged(QString const& uuid) override;
			virtual void announceMessageDeleted(QString const& uuid) override;
			virtual void announceContactChanged(openmittsu::protocol::ContactId const& contact) override;
			virtual void announceGroupChanged(openmittsu::protocol::GroupId const& group) override;
			virtual void announceNewMessage(openmittsu::protocol::ContactId const& contact, QString const& messageUuid) override;
			virtual void announceNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) override;
			virtual void announceReceivedNewMessage(openmittsu::protocol::ContactId const& contact) override;
			virtual void announceReceivedNewMessage(openmittsu::protocol::GroupId const& group) override;

			// Internal Interface
			virtual openmittsu::protocol::MessageId getNextMessageId(openmittsu::protocol::ContactId const& contact) override;
			virtual openmittsu::protocol::MessageId getNextMessageId(openmittsu::protocol::GroupId const& group) override;
			virtual QSqlQuery getQueryObject() const override;
			virtual bool transactionStart() override;
			virtual bool transactionCommit() override;
			virtual MediaFileItem getMediaItem(QString const& uuid, MediaFileType const& fileType) const override;
			virtual void insertMediaItem(QString const& uuid, QByteArray const& data, MediaFileType const& fileType) override;
			virtual void removeMediaItem(QString const& uuid, MediaFileType const& fileType) override;
			virtual void removeAllMediaItems(QString const& uuid) override;
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
			virtual QString generateUuid() const override;
			QString getCreateStatementForTable(Tables const& table);
			QStringList getUpdateStatementForTable(Tables const& table, int toVersion);
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
