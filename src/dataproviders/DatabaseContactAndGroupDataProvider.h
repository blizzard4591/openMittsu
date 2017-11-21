#ifndef OPENMITTSU_DATAPROVIDERS_DATABASECONTACTANDGROUPDATAPROVIDER_H_
#define OPENMITTSU_DATAPROVIDERS_DATABASECONTACTANDGROUPDATAPROVIDER_H_

#include "src/database/DatabaseGroupMessageCursor.h"
#include "src/dataproviders/GroupDataProvider.h"

#include <QString>
#include <QVariant>

namespace openmittsu {
	namespace database {
		class  Database;
	}

	namespace dataproviders {

		class DatabaseContactAndGroupDataProvider : public GroupDataProvider {
			Q_OBJECT
		public:
			DatabaseContactAndGroupDataProvider(openmittsu::database::Database& database);
			virtual ~DatabaseContactAndGroupDataProvider();

			// Groups
			virtual bool hasGroup(openmittsu::protocol::GroupId const& group) const override;
			virtual openmittsu::protocol::GroupStatus getGroupStatus(openmittsu::protocol::GroupId const& group) const override;
			virtual int getGroupCount() const override;
			virtual int getGroupMessageCount(openmittsu::protocol::GroupId const& group) const override;

			virtual QString getGroupTitle(openmittsu::protocol::GroupId const& group) const override;
			virtual QString getGroupDescription(openmittsu::protocol::GroupId const& group) const override;
			virtual bool getGroupHasImage(openmittsu::protocol::GroupId const& group) const override;
			virtual openmittsu::database::MediaFileItem getGroupImage(openmittsu::protocol::GroupId const& group) const override;
			virtual QSet<openmittsu::protocol::ContactId> getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const override;
			virtual bool getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const override;

			virtual void addGroup(openmittsu::protocol::GroupId const& group, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync) override;

			virtual void setGroupTitle(openmittsu::protocol::GroupId const& group, QString const& newTitle) override;
			virtual void setGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& newImage) override;
			virtual void setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) override;

			virtual BackedGroup getGroup(openmittsu::protocol::GroupId const& group, MessageCenter& messageCenter) override;
			virtual BackedGroupMessage getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid, MessageCenter& messageCenter) override;

			virtual QSet<openmittsu::protocol::GroupId> getKnownGroups() const override;
			virtual QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> getKnownGroupsWithMembersAndTitles() const override;
			virtual QSet<openmittsu::protocol::GroupId> getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const override;

			virtual std::shared_ptr <messages::GroupMessageCursor> getGroupMessageCursor(openmittsu::protocol::GroupId const& group) override;

			// Contacts
			virtual bool hasContact(openmittsu::protocol::ContactId const& contact) const override;

			virtual BackedContact getSelfContact(MessageCenter& messageCenter) override;

			virtual openmittsu::crypto::PublicKey getPublicKey(openmittsu::protocol::ContactId const& contact) const override;
			virtual QString getFirstName(openmittsu::protocol::ContactId const& contact) const override;
			virtual QString getLastName(openmittsu::protocol::ContactId const& contact) const override;
			virtual QString getNickName(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::AccountStatus getAccountStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::ContactIdVerificationStatus getVerificationStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::FeatureLevel getFeatureLevel(openmittsu::protocol::ContactId const& contact) const override;
			virtual openmittsu::protocol::ContactStatus getContactStatus(openmittsu::protocol::ContactId const& contact) const override;
			virtual int getColor(openmittsu::protocol::ContactId const& contact) const override;
			virtual int getContactCount() const override;
			virtual int getContactMessageCount(openmittsu::protocol::ContactId const& contact) const override;

			virtual void addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) override;
			virtual void addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color) override;

			virtual void setFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) override;
			virtual void setLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) override;
			virtual void setNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) override;
			virtual void setAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) override;
			virtual void setVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) override;
			virtual void setFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) override;
			virtual void setColor(openmittsu::protocol::ContactId const& contact, int color) override;

			virtual void setAccountStatusBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& status) override;
			virtual void setFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels) override;

			virtual std::shared_ptr<messages::ContactMessageCursor> getContactMessageCursor(openmittsu::protocol::ContactId const& contact) override;

			virtual BackedContact getContact(openmittsu::protocol::ContactId const& contact, MessageCenter& messageCenter) override;
			virtual BackedContactMessage getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid, MessageCenter& messageCenter) override;

			virtual QSet<openmittsu::protocol::ContactId> getKnownContacts() const override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const override;
			virtual QSet<openmittsu::protocol::ContactId> getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const override;

			virtual QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> getKnownContactsWithPublicKeys() const override;
			virtual QHash<openmittsu::protocol::ContactId, QString> getKnownContactsWithNicknames(bool withSelfContactId) const override;
		private slots:
			void onGroupChanged(openmittsu::protocol::GroupId const& group);
			void onContactChanged(openmittsu::protocol::ContactId const& contact);
			void onContactStartedTyping(openmittsu::protocol::ContactId const& identity);
			void onContactStoppedTyping(openmittsu::protocol::ContactId const& identity);
			void onContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid);
			void onGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
		private:
			openmittsu::database::Database& m_database;

			QVariant queryField(openmittsu::protocol::GroupId const& group, QString const& fieldName) const;
			
			void setFields(openmittsu::protocol::GroupId const& group, QVariantMap const& fieldsAndValues, bool doAnnounce = true);
			void setFields(openmittsu::protocol::ContactId const& contact, QVariantMap const& fieldsAndValues, bool doAnnounce = true);

			QVariant queryField(openmittsu::protocol::ContactId const& contact, QString const& fieldName) const;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_DATABASECONTACTANDGROUPDATAPROVIDER_H_
