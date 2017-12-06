#include "src/dataproviders/DatabaseContactAndGroupDataProvider.h"

#include "src/database/Database.h"
#include "src/database/DatabaseUtilities.h"
#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/protocol/ContactIdList.h"

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace dataproviders {

		DatabaseContactAndGroupDataProvider::DatabaseContactAndGroupDataProvider(openmittsu::database::Database& database) : GroupDataProvider(), m_database(database) {
			OPENMITTSU_CONNECT(&m_database, groupChanged(openmittsu::protocol::GroupId const&), this, onGroupChanged(openmittsu::protocol::GroupId const&));
			OPENMITTSU_CONNECT(&m_database, contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged(openmittsu::protocol::ContactId const&));

			OPENMITTSU_CONNECT(&m_database, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, onGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
			OPENMITTSU_CONNECT(&m_database, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, onContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));

			OPENMITTSU_CONNECT(&m_database, contactStartedTyping(openmittsu::protocol::ContactId const&), this, onContactStartedTyping(openmittsu::protocol::ContactId const&));
			OPENMITTSU_CONNECT(&m_database, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, onContactStoppedTyping(openmittsu::protocol::ContactId const&));
		}

		DatabaseContactAndGroupDataProvider::~DatabaseContactAndGroupDataProvider() {
			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, groupChanged(openmittsu::protocol::GroupId const&), this, onGroupChanged(openmittsu::protocol::GroupId const&));
			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged(openmittsu::protocol::ContactId const&));

			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, onGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, onContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));

			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, contactStartedTyping(openmittsu::protocol::ContactId const&), this, onContactStartedTyping(openmittsu::protocol::ContactId const&));
			OPENMITTSU_DISCONNECT_NOTHROW(&m_database, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, onContactStoppedTyping(openmittsu::protocol::ContactId const&));
		}

		bool DatabaseContactAndGroupDataProvider::hasGroup(openmittsu::protocol::GroupId const& group) const {
			QSqlQuery query(m_database.database);

			query.prepare(QStringLiteral("SELECT `members` FROM `groups` WHERE `id` = :groupId AND `creator` = :groupCreator AND `is_deleted` = 0"));
			query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
			query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group existance query for table groups for group " << group.toString() << ". Query error: " << query.lastError().text().toStdString();
			}

			return query.next();
		}

		openmittsu::protocol::GroupStatus DatabaseContactAndGroupDataProvider::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
			QSqlQuery query(m_database.database);

			query.prepare(QStringLiteral("SELECT `is_deleted`, `is_awaiting_sync` FROM `groups` WHERE `id` = :groupId AND `creator` = :groupCreator"));
			query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
			query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group existance query for table groups for group " << group.toString() << ". Query error: " << query.lastError().text().toStdString();
			}

			if (!query.next()) {
				return openmittsu::protocol::GroupStatus::UNKNOWN;
			}
			
			bool const isDeleted = query.value(QStringLiteral("is_deleted")).toInt() != 0;
			if (isDeleted) {
				return openmittsu::protocol::GroupStatus::DELETED;
			}

			bool const isAwaitingSync = query.value(QStringLiteral("is_awaiting_sync")).toInt() != 0;
			if (isAwaitingSync) {
				return openmittsu::protocol::GroupStatus::TEMPORARY;
			}

			return openmittsu::protocol::GroupStatus::KNOWN;
		}

		int DatabaseContactAndGroupDataProvider::getGroupCount() const {
			return openmittsu::database::DatabaseUtilities::countQuery(m_database.database, QStringLiteral("groups"));
		}

		int DatabaseContactAndGroupDataProvider::getGroupMessageCount(openmittsu::protocol::GroupId const& group) const {
			return openmittsu::database::DatabaseGroupMessage::getGroupMessageCount(m_database, group);
		}

		QString DatabaseContactAndGroupDataProvider::getGroupTitle(openmittsu::protocol::GroupId const& group) const {
			return queryField(group, QStringLiteral("groupname")).toString();
		}

		QString DatabaseContactAndGroupDataProvider::getGroupDescription(openmittsu::protocol::GroupId const& group) const {
			QSet<openmittsu::protocol::ContactId> const groupMembers = this->getGroupMembers(group, false);
			QString result;
			QSet<openmittsu::protocol::ContactId>::const_iterator it = groupMembers.constBegin();
			QSet<openmittsu::protocol::ContactId>::const_iterator end = groupMembers.constEnd();
			for (; it != end; ++it) {
				if (!result.isEmpty()) {
					result.append(QStringLiteral(", "));
				}

				result.append(this->getNickName(*it));
			}
			
			return result;
		}

		bool DatabaseContactAndGroupDataProvider::getGroupHasImage(openmittsu::protocol::GroupId const& group) const {
			QVariant const result = queryField(group, QStringLiteral("avatar_uuid"));
			if (result.isNull() || result.toString().isEmpty()) {
				return false;
			}

			return true;
		}

		openmittsu::database::MediaFileItem DatabaseContactAndGroupDataProvider::getGroupImage(openmittsu::protocol::GroupId const& group) const {
			QVariant const result = queryField(group, QStringLiteral("avatar_uuid"));
			if (result.isNull() || result.toString().isEmpty()) {
				return openmittsu::database::MediaFileItem(openmittsu::database::MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE);
			}

			return m_database.getMediaItem(result.toString());
		}

		QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
			QSqlQuery query(m_database.database);
			openmittsu::protocol::ContactId const selfContact = m_database.getSelfContact();

			query.prepare(QStringLiteral("SELECT `members` FROM `groups` WHERE `id` = :id AND `creator` = :creator;"));
			query.bindValue(QStringLiteral(":id"), QVariant(group.groupIdWithoutOwnerToQString()));
			query.bindValue(QStringLiteral(":creator"), QVariant(group.getOwner().toQString()));

			if (query.exec() && query.isSelect()) {
				if (!query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group member enumeration query for group " << group.toString() << " on table groups. Group does not exist!";
				}

				QString const memberString(query.value(QStringLiteral("members")).toString());
				QSet<openmittsu::protocol::ContactId> result = openmittsu::protocol::ContactIdList::fromString(memberString).getContactIds();
				
				if (excludeSelfContact) {
					result.remove(selfContact);
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group member enumeration query for group " << group.toString() << " on table groups. Query error: " << query.lastError().text().toStdString();
			}
		}

		bool DatabaseContactAndGroupDataProvider::getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const {
			return queryField(group, QStringLiteral("is_awaiting_sync")).toBool();
		}

		void DatabaseContactAndGroupDataProvider::addGroup(openmittsu::protocol::GroupId const& group, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync) {
			openmittsu::protocol::GroupStatus const groupStatus = this->getGroupStatus(group);
			if ((groupStatus == openmittsu::protocol::GroupStatus::DELETED) || (groupStatus == openmittsu::protocol::GroupStatus::TEMPORARY) || (groupStatus == openmittsu::protocol::GroupStatus::KNOWN)) {
				setGroupMembers(group, members);
				setFields(group, { {QStringLiteral("is_awaiting_sync"), isAwaitingSync} });
			} else {
				openmittsu::protocol::ContactId const ourId = m_database.getSelfContact();
				bool containsUs = members.contains(ourId);
				QString const memberString = openmittsu::protocol::ContactIdList(members).toString();
				int const isDeletedInt = (containsUs && (!isDeleted)) ? 0 : 1;

				QSqlQuery query(m_database.database);
				query.prepare(QStringLiteral("INSERT INTO `groups` (`id`, `creator`, `groupname`, `created_at`, `members`, `avatar_uuid`, `is_deleted`, `is_awaiting_sync`) VALUES "
											 "(:groupId, :groupCreator, :groupName, :createdAt, :members, :avatarUuid, :isDeleted, :isAwaitingSync);"));
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));
				query.bindValue(QStringLiteral(":groupName"), QVariant(name));
				query.bindValue(QStringLiteral(":createdAt"), QVariant(createdAt.getMessageTimeMSecs()));
				query.bindValue(QStringLiteral(":members"), QVariant(memberString));
				query.bindValue(QStringLiteral(":avatarUuid"), QVariant(""));
				query.bindValue(QStringLiteral(":isDeleted"), QVariant(isDeletedInt));
				query.bindValue(QStringLiteral(":isAwaitingSync"), QVariant(isAwaitingSync));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert group into 'groups'. Query error: " << query.lastError().text().toStdString();
				}

				m_database.announceGroupChanged(group);
			}
		}

		void DatabaseContactAndGroupDataProvider::setGroupTitle(openmittsu::protocol::GroupId const& group, QString const& newTitle) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not set title \"" << newTitle.toStdString() << "\", the given group " << group.toString() << " is unknown!";
			}

			setFields(group, { {QStringLiteral("groupname"), newTitle} });
		}

		void DatabaseContactAndGroupDataProvider::setGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& newImage) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not set group image, the given group " << group.toString() << " is unknown!";
			}

			QVariant const oldUuid = queryField(group, QStringLiteral("avatar_uuid"));
			if (!oldUuid.isNull() && !oldUuid.toString().isEmpty()) {
				m_database.removeMediaItem(oldUuid.toString());
			}

			QString const uuid = m_database.insertMediaItem(newImage);
			setFields(group, { {QStringLiteral("avatar_uuid"), uuid} });

			m_database.announceGroupChanged(group);
		}

		void DatabaseContactAndGroupDataProvider::setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) {
			openmittsu::protocol::ContactId const ourId = m_database.getSelfContact();
			bool containsUs = newMembers.contains(ourId);

			QString const memberString(openmittsu::protocol::ContactIdList(newMembers).toString());
			int const isDeleted = (containsUs) ? 0 : 1;

			setFields(group, { {QStringLiteral("members"), memberString}, {QStringLiteral("is_deleted"), isDeleted} });

			m_database.announceGroupChanged(group);
		}

		BackedGroup DatabaseContactAndGroupDataProvider::getGroup(openmittsu::protocol::GroupId const& group, MessageCenter& messageCenter) {
			return BackedGroup(group, *this, *this, messageCenter);
		}

		QSet<openmittsu::protocol::GroupId> DatabaseContactAndGroupDataProvider::getKnownGroups() const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `id`, `creator` FROM `groups` WHERE `is_deleted` = 0"));

			if (query.exec() && query.isSelect()) {
				QSet<openmittsu::protocol::GroupId> result;
				while (query.next()) {
					QString const groupId = query.value(QStringLiteral("id")).toString();
					openmittsu::protocol::ContactId const creator(query.value(QStringLiteral("creator")).toString());
					openmittsu::protocol::GroupId const group(creator, groupId);

					result.insert(group);
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group enumeration query for table groups. Query error: " << query.lastError().text().toStdString();
			}
		}

		QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> DatabaseContactAndGroupDataProvider::getKnownGroupsWithMembersAndTitles() const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `id`, `creator`, `groupname`, `members` FROM `groups` WHERE `is_deleted` = 0"));

			if (query.exec() && query.isSelect()) {
				QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> result;
				while (query.next()) {
					QString const groupId = query.value(QStringLiteral("id")).toString();
					openmittsu::protocol::ContactId const creator(query.value(QStringLiteral("creator")).toString());
					openmittsu::protocol::GroupId const group(creator, groupId);

					QString const title(query.value(QStringLiteral("groupname")).toString());
					QString const memberString(query.value(QStringLiteral("members")).toString());
					QSet<openmittsu::protocol::ContactId> const members = openmittsu::protocol::ContactIdList::fromString(memberString).getContactIds();
					result.insert(group, std::make_pair(members, title));
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group enumeration query with title and members for table groups. Query error: " << query.lastError().text().toStdString();
			}
		}

		QSet<openmittsu::protocol::GroupId> DatabaseContactAndGroupDataProvider::getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const {
			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `id`, `creator`, `members` FROM `groups` WHERE `is_deleted` = 0"));

			if (query.exec() && query.isSelect()) {
				QSet<openmittsu::protocol::GroupId> result;
				QString const keyMember = identity.toQString();

				while (query.next()) {
					QString const memberString(query.value(QStringLiteral("members")).toString());
					if (memberString.contains(keyMember, Qt::CaseInsensitive)) {
						QString const groupId = query.value(QStringLiteral("id")).toString();
						openmittsu::protocol::ContactId const creator(query.value(QStringLiteral("creator")).toString());
						openmittsu::protocol::GroupId const group(creator, groupId);

						result.insert(group);
					}
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute group enumeration query for table groups for key memver " << identity.toString() << ". Query error: " << query.lastError().text().toStdString();
			}
		}

		void DatabaseContactAndGroupDataProvider::onGroupChanged(openmittsu::protocol::GroupId const& group) {
			emit groupChanged(group);
		}

		void DatabaseContactAndGroupDataProvider::onContactStartedTyping(openmittsu::protocol::ContactId const& identity) {
			emit contactStartedTyping(identity);
		}

		void DatabaseContactAndGroupDataProvider::onContactStoppedTyping(openmittsu::protocol::ContactId const& identity) {
			emit contactStoppedTyping(identity);
		}

		void DatabaseContactAndGroupDataProvider::onContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid) {
			emit contactHasNewMessage(identity, messageUuid);
		}

		void DatabaseContactAndGroupDataProvider::onGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) {
			emit groupHasNewMessage(group, messageUuid);
		}

		QVariant DatabaseContactAndGroupDataProvider::queryField(openmittsu::protocol::GroupId const& group, QString const& fieldName) const {
			QSqlQuery query(m_database.database);

			query.prepare(QStringLiteral("SELECT `%1` FROM `groups` WHERE `id` = :groupId AND `creator` = :groupCreator;").arg(fieldName));
			query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
			query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute message field query for table groups with group \"" << group.toString() << "\" for field \"" << fieldName.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
			} else if (!query.next()) {
				throw openmittsu::exceptions::InternalErrorException() << "No group with group ID \"" << group.toString() << "\" exists, can not manipulate.";
			}

			return query.value(fieldName);
		}

		void DatabaseContactAndGroupDataProvider::setFields(openmittsu::protocol::GroupId const& group, QVariantMap const& fieldsAndValues, bool doAnnounce) {
			if (fieldsAndValues.size() > 0) {
				QSqlQuery query(m_database.database);

				openmittsu::database::DatabaseUtilities::prepareSetFieldsUpdateQuery(query, QStringLiteral("UPDATE `groups` SET %1 WHERE `id` = :groupId AND `creator` = :groupCreator;"), fieldsAndValues);
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not update group data for group ID \"" << group.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}

				if (doAnnounce) {
					m_database.announceGroupChanged(group);
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "DatabaseContactAndGroupDataProvider::setFields() called with empty field/value map, this should never happen!";
			}
		}

		void DatabaseContactAndGroupDataProvider::setFields(openmittsu::protocol::ContactId const& contact, QVariantMap const& fieldsAndValues, bool doAnnounce) {
			if (fieldsAndValues.size() > 0) {
				QSqlQuery query(m_database.database);

				openmittsu::database::DatabaseUtilities::prepareSetFieldsUpdateQuery(query, QStringLiteral("UPDATE `contacts` SET %1 WHERE `identity` = :identity;"), fieldsAndValues);
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not update contact data for group ID \"" << contact.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}

				if (doAnnounce) {
					m_database.announceContactChanged(contact);
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "DatabaseContactAndGroupDataProvider::setFields() called with empty field/value map, this should never happen!";
			}
		}

		QVariant DatabaseContactAndGroupDataProvider::queryField(openmittsu::protocol::ContactId const& contact, QString const& fieldName) const {
			QSqlQuery query(m_database.database);

			query.prepare(QStringLiteral("SELECT `%1` FROM `contacts` WHERE `identity` = :identity;").arg(fieldName));
			query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact field query for field \"" << fieldName.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
			} else if (!query.next()) {
				throw openmittsu::exceptions::InternalErrorException() << "No contact with identity \"" << contact.toString() << "\" exists, can not manipulate.";
			}

			return query.value(fieldName);
		}

		std::shared_ptr<messages::GroupMessageCursor> DatabaseContactAndGroupDataProvider::getGroupMessageCursor(openmittsu::protocol::GroupId const& group) {
			return std::make_shared<openmittsu::database::DatabaseGroupMessageCursor>(m_database, group);
		}

		BackedGroupMessage DatabaseContactAndGroupDataProvider::getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid, MessageCenter& messageCenter) {
			openmittsu::database::DatabaseGroupMessageCursor cursor(m_database, group);
			if (!cursor.seekByUuid(uuid)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not find message with UUID " << uuid.toStdString() << " for group " << group.toString() << ".";
			}
			auto message = cursor.getMessage();

			return BackedGroupMessage(message, getContact(message->getSender(), messageCenter), messageCenter);
		}


		// Contacts
		bool DatabaseContactAndGroupDataProvider::hasContact(openmittsu::protocol::ContactId const& contact) const {
			QSqlQuery query(m_database.database);

			query.prepare(QStringLiteral("SELECT `identity` FROM `contacts` WHERE `identity` = :identity;"));
			query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact existance query for contact \"" << contact.toString() << "\". Query error: " << query.lastError().text().toStdString();
			}

			return query.next();
		}

		BackedContact DatabaseContactAndGroupDataProvider::getSelfContact(MessageCenter& messageCenter) {
			return getContact(m_database.getSelfContact(), messageCenter);
		}

		openmittsu::crypto::PublicKey DatabaseContactAndGroupDataProvider::getPublicKey(openmittsu::protocol::ContactId const& contact) const {
			return openmittsu::crypto::PublicKey::fromHexString(queryField(contact, QStringLiteral("publickey")).toString());
		}

		QString DatabaseContactAndGroupDataProvider::getFirstName(openmittsu::protocol::ContactId const& contact) const {
			return queryField(contact, QStringLiteral("firstname")).toString();
		}

		QString DatabaseContactAndGroupDataProvider::getLastName(openmittsu::protocol::ContactId const& contact) const {
			return queryField(contact, QStringLiteral("lastname")).toString();
		}

		QString DatabaseContactAndGroupDataProvider::getNickName(openmittsu::protocol::ContactId const& contact) const {
			return queryField(contact, QStringLiteral("nick_name")).toString();
		}

		openmittsu::protocol::AccountStatus DatabaseContactAndGroupDataProvider::getAccountStatus(openmittsu::protocol::ContactId const& contact) const {
			return openmittsu::protocol::AccountStatusHelper::fromInt(queryField(contact, QStringLiteral("status")).toInt());
		}

		openmittsu::protocol::ContactIdVerificationStatus DatabaseContactAndGroupDataProvider::getVerificationStatus(openmittsu::protocol::ContactId const& contact) const {
			return openmittsu::protocol::ContactIdVerificationStatusHelper::fromQString(queryField(contact, QStringLiteral("verification")).toString());
		}

		openmittsu::protocol::FeatureLevel DatabaseContactAndGroupDataProvider::getFeatureLevel(openmittsu::protocol::ContactId const& contact) const {
			return openmittsu::protocol::FeatureLevelHelper::fromInt(queryField(contact, QStringLiteral("feature_level")).toInt());
		}

		openmittsu::protocol::ContactStatus DatabaseContactAndGroupDataProvider::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
			if (hasContact(contact)) {
				return openmittsu::protocol::ContactStatus::KNOWN;
			} else {
				return openmittsu::protocol::ContactStatus::UNKNOWN;
			}
		}

		int DatabaseContactAndGroupDataProvider::getColor(openmittsu::protocol::ContactId const& contact) const {
			return queryField(contact, QStringLiteral("color")).toInt();
		}

		int DatabaseContactAndGroupDataProvider::getContactCount() const {
			return openmittsu::database::DatabaseUtilities::countQuery(m_database.database, QStringLiteral("contacts"));
		}

		int DatabaseContactAndGroupDataProvider::getContactMessageCount(openmittsu::protocol::ContactId const& contact) const {
			return openmittsu::database::DatabaseContactMessage::getContactMessageCount(m_database, contact);
		}

		void DatabaseContactAndGroupDataProvider::addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
			addContact(contact, publicKey, openmittsu::protocol::ContactIdVerificationStatus::VERIFICATION_STATUS_UNVERIFIED, QStringLiteral(""), QStringLiteral(""), QStringLiteral(""), 0);
		}
		
		void DatabaseContactAndGroupDataProvider::addContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color) {
			openmittsu::protocol::ContactStatus const contactStatus = this->getContactStatus(contact);
			if ((contactStatus == openmittsu::protocol::ContactStatus::DELETED) || (contactStatus == openmittsu::protocol::ContactStatus::KNOWN)) {
				if (getPublicKey(contact) != publicKey) {
					throw openmittsu::exceptions::InternalErrorException() << "Can not create contact, inconsistent data: Contact " << contact.toString() << " already exists with public key " << getPublicKey(contact).toString() << ", which is different from the new public key " << publicKey.toString() << "!";
				}
				setVerificationStatus(contact, verificationStatus);
				setFirstName(contact, firstName);
				setLastName(contact, lastName);
				setNickName(contact, nickName);
				setColor(contact, color);
			} else {
				QSqlQuery query(m_database.database);
				query.prepare(QStringLiteral("INSERT INTO `contacts` (`identity`, `publickey`, `verification`, `acid`, `tacid`, `firstname`, `lastname`, `nick_name`, `color`, `status`, `status_last_check`, `feature_level`, `feature_level_last_check`) VALUES "
											 "(:identity, :publickey, :verificationStatus, '', '', :firstName, :lastName, :nickName, :color, :status, -1, :featureLevel, -1);"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));
				query.bindValue(QStringLiteral(":publickey"), QVariant(QString(publicKey.getPublicKey().toHex())));
				query.bindValue(QStringLiteral(":verificationStatus"), QVariant(openmittsu::protocol::ContactIdVerificationStatusHelper::toQString(verificationStatus)));
				query.bindValue(QStringLiteral(":firstName"), QVariant(firstName));
				query.bindValue(QStringLiteral(":lastName"), QVariant(lastName));
				query.bindValue(QStringLiteral(":nickName"), QVariant(nickName));
				query.bindValue(QStringLiteral(":color"), QVariant(color));
				query.bindValue(QStringLiteral(":status"), QVariant(openmittsu::protocol::AccountStatusHelper::toInt(openmittsu::protocol::AccountStatus::STATUS_UNKNOWN)));
				query.bindValue(QStringLiteral(":featureLevel"), QVariant(openmittsu::protocol::FeatureLevelHelper::toInt(openmittsu::protocol::FeatureLevel::LEVEL_UNKNOW)));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert contact into 'contacts'. Query error: " << query.lastError().text().toStdString();
				}
			}
		}

		void DatabaseContactAndGroupDataProvider::setNickName(openmittsu::protocol::ContactId const& contact, QString const& nickname) {
			setFields(contact, { {QStringLiteral("nick_name"), nickname} });
		}

		void DatabaseContactAndGroupDataProvider::setFirstName(openmittsu::protocol::ContactId const& contact, QString const& firstName) {
			setFields(contact, { {QStringLiteral("firstname"), firstName} });
		}

		void DatabaseContactAndGroupDataProvider::setLastName(openmittsu::protocol::ContactId const& contact, QString const& lastName) {
			setFields(contact, { {QStringLiteral("lastname"), lastName} });
		}

		void DatabaseContactAndGroupDataProvider::setAccountStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::AccountStatus const& status) {
			setFields(contact, { {QStringLiteral("status"), openmittsu::protocol::AccountStatusHelper::toInt(status)}, { QStringLiteral("status_last_check"), openmittsu::protocol::MessageTime::now().getMessageTimeMSecs()} });
		}

		void DatabaseContactAndGroupDataProvider::setVerificationStatus(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) {
			setFields(contact, { {QStringLiteral("verification"), openmittsu::protocol::ContactIdVerificationStatusHelper::toQString(verificationStatus)} });
		}

		void DatabaseContactAndGroupDataProvider::setFeatureLevel(openmittsu::protocol::ContactId const& contact, openmittsu::protocol::FeatureLevel const& featureLevel) {
			setFields(contact, { {QStringLiteral("feature_level"), openmittsu::protocol::FeatureLevelHelper::toInt(featureLevel)}, {QStringLiteral("feature_level_last_check"), openmittsu::protocol::MessageTime::now().getMessageTimeMSecs()} });
		}

		void DatabaseContactAndGroupDataProvider::setColor(openmittsu::protocol::ContactId const& contact, int color) {
			setFields(contact, { {QStringLiteral("color"), color} });
		}

		void DatabaseContactAndGroupDataProvider::setAccountStatusBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& status) {
			auto it = status.constBegin();
			auto end = status.constEnd();
			qint64 const timeNow = openmittsu::protocol::MessageTime::now().getMessageTimeMSecs();

			m_database.database.transaction();
			for (; it != end; ++it) {
				setFields(it.key(), { {QStringLiteral("status"), openmittsu::protocol::AccountStatusHelper::toInt(it.value())}, {QStringLiteral("status_last_check"), timeNow} }, false);
			}
			m_database.database.commit();

			// TODO: Fixme. This might be broken
			m_database.announceContactChanged(m_database.getSelfContact());
		}
		
		void DatabaseContactAndGroupDataProvider::setFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels) {
			auto it = featureLevels.constBegin();
			auto end = featureLevels.constEnd();
			qint64 const timeNow = openmittsu::protocol::MessageTime::now().getMessageTimeMSecs();

			m_database.database.transaction();
			for (; it != end; ++it) {
				setFields(it.key(), { {QStringLiteral("feature_level"), openmittsu::protocol::FeatureLevelHelper::toInt(it.value())}, {QStringLiteral("feature_level_last_check"), timeNow} }, false);
			}
			m_database.database.commit();

			// TODO: Fixme. This might be broken
			m_database.announceContactChanged(m_database.getSelfContact());
		}

		BackedContact DatabaseContactAndGroupDataProvider::getContact(openmittsu::protocol::ContactId const& contact, MessageCenter& messageCenter) {
			return BackedContact(contact, getPublicKey(contact), *this, messageCenter);
		}

		QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getKnownContacts() const {
			QSet<openmittsu::protocol::ContactId> result;

			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `identity` FROM `contacts`;"));

			if (query.exec() && query.isSelect()) {
				while (query.next()) {
					result.insert(openmittsu::protocol::ContactId(query.value(QStringLiteral("identity")).toString()));
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact enumeration query for table contacts. Query error: " << query.lastError().text().toStdString();
			}
		}

		QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const {
			QSet<openmittsu::protocol::ContactId> result;
			QSqlQuery query(m_database.database);
			openmittsu::protocol::MessageTime const limit(QDateTime::currentDateTime().addSecs(-maximalAgeInSeconds));

			query.prepare(QStringLiteral("SELECT `identity` FROM `contacts` WHERE ((`feature_level_last_check` <= %1) OR (`feature_level_last_check` IS NULL));").arg(limit.getMessageTimeMSecs()));
			if (query.exec() && query.isSelect()) {
				while (query.next()) {
					result.insert(openmittsu::protocol::ContactId(query.value(QStringLiteral("identity")).toString()));
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contacts requiring feature level check enumeration query for table contacts. Query error: " << query.lastError().text().toStdString();
			}
		}

		QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const {
			QSet<openmittsu::protocol::ContactId> result;
			QSqlQuery query(m_database.database);
			openmittsu::protocol::MessageTime const limit(QDateTime::currentDateTime().addSecs(-maximalAgeInSeconds));

			query.prepare(QStringLiteral("SELECT `identity` FROM `contacts` WHERE ((`status_last_check` <= %1) OR (`status_last_check` IS NULL));").arg(limit.getMessageTimeMSecs()));
			if (query.exec() && query.isSelect()) {
				while (query.next()) {
					result.insert(openmittsu::protocol::ContactId(query.value(QStringLiteral("identity")).toString()));
				}
				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contacts requiring feature level check enumeration query for table contacts. Query error: " << query.lastError().text().toStdString();
			}
		}

		QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> DatabaseContactAndGroupDataProvider::getKnownContactsWithPublicKeys() const {
			QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> result;

			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `identity`, `publickey` FROM `contacts`;"));

			if (query.exec() && query.isSelect()) {
				while (query.next()) {
					openmittsu::protocol::ContactId const contactId(query.value(QStringLiteral("identity")).toString());
					openmittsu::crypto::PublicKey const publicKey(openmittsu::crypto::PublicKey::fromHexString(query.value(QStringLiteral("publickey")).toString()));

					result.insert(contactId, publicKey);
				}

				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact/key enumeration query for table contacts. Query error: " << query.lastError().text().toStdString();
			}
		}

		QHash<openmittsu::protocol::ContactId, QString> DatabaseContactAndGroupDataProvider::getKnownContactsWithNicknames(bool withSelfContactId) const {
			QHash<openmittsu::protocol::ContactId, QString> result;
			openmittsu::protocol::ContactId const selfContact = m_database.getSelfContact();

			QSqlQuery query(m_database.database);
			query.prepare(QStringLiteral("SELECT `identity`, `firstname`, `lastname`, `nick_name` FROM `contacts`;"));

			if (query.exec() && query.isSelect()) {
				while (query.next()) {
					openmittsu::protocol::ContactId const contactId(query.value(QStringLiteral("identity")).toString());
					QString nickname(query.value(QStringLiteral("nick_name")).toString());
					if (nickname.isNull() || nickname.isEmpty()) {
						QString const firstname = query.value(QStringLiteral("firstname")).toString();
						QString const lastname = query.value(QStringLiteral("lastname")).toString();
						if (!firstname.isEmpty() || !lastname.isEmpty()) {
							nickname = QString(firstname).append(" ").append(lastname);
						} else {
							nickname = contactId.toQString();
						}
					}

					if ((!withSelfContactId) && (selfContact == contactId)) {
						continue;
					}

					result.insert(contactId, nickname);
				}

				if (withSelfContactId && (!result.contains(selfContact))) {
					result.insert(selfContact, QStringLiteral("You"));
				}

				return result;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact/nickname enumeration query for table contacts. Query error: " << query.lastError().text().toStdString();
			}
		}

		void DatabaseContactAndGroupDataProvider::onContactChanged(openmittsu::protocol::ContactId const& identity) {
			emit contactChanged(identity);
		}

		std::shared_ptr<messages::ContactMessageCursor> DatabaseContactAndGroupDataProvider::getContactMessageCursor(openmittsu::protocol::ContactId const& contact) {
			return std::make_shared<openmittsu::database::DatabaseContactMessageCursor>(m_database, contact);
		}

		BackedContactMessage DatabaseContactAndGroupDataProvider::getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid, MessageCenter& messageCenter) {
			openmittsu::database::DatabaseContactMessageCursor cursor(m_database, contact);
			if (!cursor.seekByUuid(uuid)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not find message with UUID " << uuid.toStdString() << " for contact " << contact.toString() << ".";
			}

			auto message = cursor.getMessage();
			return BackedContactMessage(message, this->getContact(message->getSender(), messageCenter), messageCenter);
		}
	}
}
