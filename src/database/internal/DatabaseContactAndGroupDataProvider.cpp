#include "src/database/internal/DatabaseContactAndGroupDataProvider.h"

#include "src/database/Database.h"
#include "src/database/internal/DatabaseContactMessageCursor.h"
#include "src/database/internal/DatabaseUtilities.h"
#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/protocol/ContactIdList.h"

#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace database {
		namespace internal {

			DatabaseContactAndGroupDataProvider::DatabaseContactAndGroupDataProvider(Database* signalSource, InternalDatabaseInterface* database) : GroupDataProvider(), m_signalSource(signalSource), m_database(database) {
				OPENMITTSU_CONNECT(m_signalSource, groupChanged(openmittsu::protocol::GroupId const&), this, onGroupChanged(openmittsu::protocol::GroupId const&));
				OPENMITTSU_CONNECT(m_signalSource, contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged(openmittsu::protocol::ContactId const&));

				OPENMITTSU_CONNECT(m_signalSource, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, onGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
				OPENMITTSU_CONNECT(m_signalSource, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, onContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));

				OPENMITTSU_CONNECT(m_signalSource, contactStartedTyping(openmittsu::protocol::ContactId const&), this, onContactStartedTyping(openmittsu::protocol::ContactId const&));
				OPENMITTSU_CONNECT(m_signalSource, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, onContactStoppedTyping(openmittsu::protocol::ContactId const&));
			}

			DatabaseContactAndGroupDataProvider::~DatabaseContactAndGroupDataProvider() {
				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, groupChanged(openmittsu::protocol::GroupId const&), this, onGroupChanged(openmittsu::protocol::GroupId const&));
				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, contactChanged(openmittsu::protocol::ContactId const&), this, onContactChanged(openmittsu::protocol::ContactId const&));

				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, onGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, onContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));

				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, contactStartedTyping(openmittsu::protocol::ContactId const&), this, onContactStartedTyping(openmittsu::protocol::ContactId const&));
				OPENMITTSU_DISCONNECT_NOTHROW(m_signalSource, contactStoppedTyping(openmittsu::protocol::ContactId const&), this, onContactStoppedTyping(openmittsu::protocol::ContactId const&));
			}

			bool DatabaseContactAndGroupDataProvider::hasGroup(openmittsu::protocol::GroupId const& group) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `members` FROM `groups` WHERE `id` = :groupId AND `creator` = :groupCreator AND `is_deleted` = 0"));
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group existance query for table groups for group " << group.toString() << ". Query error: " << query.lastError().text().toStdString();
				}

				return query.next();
			}

			openmittsu::protocol::GroupStatus DatabaseContactAndGroupDataProvider::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
				QSqlQuery query(m_database->getQueryObject());

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
				return openmittsu::database::internal::DatabaseUtilities::countQuery(m_database, QStringLiteral("groups"));
			}

			openmittsu::database::MediaFileItem DatabaseContactAndGroupDataProvider::getGroupImage(openmittsu::protocol::GroupId const& group) const {
				QVariant const result = queryField(group, QStringLiteral("avatar_uuid"));
				if (result.isNull() || result.toString().isEmpty()) {
					return openmittsu::database::MediaFileItem(openmittsu::database::MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE);
				}

				return m_database->getMediaItem(result.toString());
			}

			QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
				QSqlQuery query(m_database->getQueryObject());
				openmittsu::protocol::ContactId const selfContact = m_database->getSelfContact();

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

			void DatabaseContactAndGroupDataProvider::addGroup(QVector<openmittsu::database::NewGroupData> const& newGroupData) {
				auto it = newGroupData.constBegin();
				auto const end = newGroupData.constEnd();

				while (it != end) {
					openmittsu::protocol::GroupStatus const groupStatus = this->getGroupStatus(it->id);
					if ((groupStatus == openmittsu::protocol::GroupStatus::DELETED) || (groupStatus == openmittsu::protocol::GroupStatus::TEMPORARY) || (groupStatus == openmittsu::protocol::GroupStatus::KNOWN)) {
						openmittsu::protocol::ContactId const ourId = m_database->getSelfContact();
						bool containsUs = it->members.contains(ourId);
						QString const memberString = openmittsu::protocol::ContactIdList(it->members).toString();
						int const isDeletedInt = (containsUs && (!it->isDeleted)) ? 0 : 1;

						QSqlQuery query(m_database->getQueryObject());
						query.prepare(QStringLiteral("UPDATE `groups` SET `groupname` = :groupName, `members` = :members, `is_deleted` = :isDeleted, `is_awaiting_sync` = :isAwaitingSync WHERE `id` = :groupId AND `creator` = :groupCreator;"));
						query.bindValue(QStringLiteral(":groupId"), QVariant(it->id.groupIdWithoutOwnerToQString()));
						query.bindValue(QStringLiteral(":groupCreator"), QVariant(it->id.getOwner().toQString()));
						query.bindValue(QStringLiteral(":groupName"), QVariant(it->name));
						query.bindValue(QStringLiteral(":members"), QVariant(memberString));
						query.bindValue(QStringLiteral(":isDeleted"), QVariant(isDeletedInt));
						query.bindValue(QStringLiteral(":isAwaitingSync"), QVariant(it->isAwaitingSync));

						if (!query.exec()) {
							throw openmittsu::exceptions::InternalErrorException() << "Could not update group data for group ID \"" << it->id.toString() << "\". Query error: " << query.lastError().text().toStdString();
						}

						m_database->announceGroupChanged(it->id);
					} else {
						openmittsu::protocol::ContactId const ourId = m_database->getSelfContact();
						bool containsUs = it->members.contains(ourId);
						QString const memberString = openmittsu::protocol::ContactIdList(it->members).toString();
						int const isDeletedInt = (containsUs && (!it->isDeleted)) ? 0 : 1;

						QSqlQuery query(m_database->getQueryObject());
						query.prepare(QStringLiteral("INSERT INTO `groups` (`id`, `creator`, `groupname`, `created_at`, `members`, `avatar_uuid`, `is_deleted`, `is_awaiting_sync`) VALUES "
													 "(:groupId, :groupCreator, :groupName, :createdAt, :members, :avatarUuid, :isDeleted, :isAwaitingSync);"));
						query.bindValue(QStringLiteral(":groupId"), QVariant(it->id.groupIdWithoutOwnerToQString()));
						query.bindValue(QStringLiteral(":groupCreator"), QVariant(it->id.getOwner().toQString()));
						query.bindValue(QStringLiteral(":groupName"), QVariant(it->name));
						query.bindValue(QStringLiteral(":createdAt"), QVariant(it->createdAt.getMessageTimeMSecs()));
						query.bindValue(QStringLiteral(":members"), QVariant(memberString));
						query.bindValue(QStringLiteral(":avatarUuid"), QVariant(""));
						query.bindValue(QStringLiteral(":isDeleted"), QVariant(isDeletedInt));
						query.bindValue(QStringLiteral(":isAwaitingSync"), QVariant(it->isAwaitingSync));

						if (!query.exec()) {
							throw openmittsu::exceptions::InternalErrorException() << "Could not insert group into 'groups'. Query error: " << query.lastError().text().toStdString();
						}

						m_database->announceGroupChanged(it->id);
					}
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
					m_database->removeMediaItem(oldUuid.toString());
				}

				QString const uuid = m_database->insertMediaItem(newImage);
				setFields(group, { {QStringLiteral("avatar_uuid"), uuid} });

				m_database->announceGroupChanged(group);
			}

			void DatabaseContactAndGroupDataProvider::setGroupMembers(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& newMembers) {
				openmittsu::protocol::ContactId const ourId = m_database->getSelfContact();
				bool containsUs = newMembers.contains(ourId);

				QString const memberString(openmittsu::protocol::ContactIdList(newMembers).toString());
				int const isDeleted = (containsUs) ? 0 : 1;

				setFields(group, { {QStringLiteral("members"), memberString}, {QStringLiteral("is_deleted"), isDeleted} });

				m_database->announceGroupChanged(group);
			}

			QSet<openmittsu::protocol::GroupId> DatabaseContactAndGroupDataProvider::getKnownGroups() const {
				QSqlQuery query(m_database->getQueryObject());
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
				QSqlQuery query(m_database->getQueryObject());
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

			QHash<openmittsu::protocol::GroupId, QString> DatabaseContactAndGroupDataProvider::getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const {
				QSqlQuery query(m_database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `id`, `creator`, `groupname`, `members` FROM `groups` WHERE `is_deleted` = 0"));

				if (query.exec() && query.isSelect()) {
					QHash<openmittsu::protocol::GroupId, QString> result;
					QString const keyMember = identity.toQString();

					while (query.next()) {
						QString const memberString(query.value(QStringLiteral("members")).toString());
						if (memberString.contains(keyMember, Qt::CaseInsensitive)) {
							QString const groupId = query.value(QStringLiteral("id")).toString();
							openmittsu::protocol::ContactId const creator(query.value(QStringLiteral("creator")).toString());
							openmittsu::protocol::GroupId const group(creator, groupId);
							QString const title(query.value(QStringLiteral("groupname")).toString());

							result.insert(group, title);
						}
					}
					return result;
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group enumeration query for table groups for key member " << identity.toString() << ". Query error: " << query.lastError().text().toStdString();
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
				QSqlQuery query(m_database->getQueryObject());

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
					QSqlQuery query(m_database->getQueryObject());

					openmittsu::database::internal::DatabaseUtilities::prepareSetFieldsUpdateQuery(query, QStringLiteral("UPDATE `groups` SET %1 WHERE `id` = :groupId AND `creator` = :groupCreator;"), fieldsAndValues);
					query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
					query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

					if (!query.exec()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not update group data for group ID \"" << group.toString() << "\". Query error: " << query.lastError().text().toStdString();
					}

					if (doAnnounce) {
						m_database->announceGroupChanged(group);
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "DatabaseContactAndGroupDataProvider::setFields() called with empty field/value map, this should never happen!";
				}
			}

			void DatabaseContactAndGroupDataProvider::setFields(openmittsu::protocol::ContactId const& contact, QVariantMap const& fieldsAndValues, bool doAnnounce) {
				if (fieldsAndValues.size() > 0) {
					QSqlQuery query(m_database->getQueryObject());

					openmittsu::database::internal::DatabaseUtilities::prepareSetFieldsUpdateQuery(query, QStringLiteral("UPDATE `contacts` SET %1 WHERE `identity` = :identity;"), fieldsAndValues);
					query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

					if (!query.exec()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not update contact data for group ID \"" << contact.toString() << "\". Query error: " << query.lastError().text().toStdString();
					}

					if (doAnnounce) {
						m_database->announceContactChanged(contact);
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "DatabaseContactAndGroupDataProvider::setFields() called with empty field/value map, this should never happen!";
				}
			}

			QVariant DatabaseContactAndGroupDataProvider::queryField(openmittsu::protocol::ContactId const& contact, QString const& fieldName) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `%1` FROM `contacts` WHERE `identity` = :identity;").arg(fieldName));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact field query for field \"" << fieldName.toStdString() << "\". Query error: " << query.lastError().text().toStdString();
				} else if (!query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "No contact with identity \"" << contact.toString() << "\" exists, can not manipulate.";
				}

				return query.value(fieldName);
			}

			std::shared_ptr<openmittsu::dataproviders::messages::GroupMessageCursor> DatabaseContactAndGroupDataProvider::getGroupMessageCursor(openmittsu::protocol::GroupId const& group) {
				return std::make_shared<openmittsu::database::internal::DatabaseGroupMessageCursor>(m_database, group);
			}

			// Contacts
			bool DatabaseContactAndGroupDataProvider::hasContact(openmittsu::protocol::ContactId const& contact) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `identity` FROM `contacts` WHERE `identity` = :identity;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact existance query for contact \"" << contact.toString() << "\". Query error: " << query.lastError().text().toStdString();
				}

				return query.next();
			}

			openmittsu::crypto::PublicKey DatabaseContactAndGroupDataProvider::getPublicKey(openmittsu::protocol::ContactId const& contact) const {
				return openmittsu::crypto::PublicKey::fromHexString(queryField(contact, QStringLiteral("publickey")).toString());
			}

			openmittsu::protocol::ContactStatus DatabaseContactAndGroupDataProvider::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
				if (hasContact(contact)) {
					return openmittsu::protocol::ContactStatus::KNOWN;
				} else {
					return openmittsu::protocol::ContactStatus::UNKNOWN;
				}
			}

			int DatabaseContactAndGroupDataProvider::getContactCount() const {
				return openmittsu::database::internal::DatabaseUtilities::countQuery(m_database, QStringLiteral("contacts"));
			}

			void DatabaseContactAndGroupDataProvider::addContact(QVector<openmittsu::database::NewContactData> const& newContactData) {
				auto it = newContactData.constBegin();
				auto const end = newContactData.constEnd();

				while (it != end) {
					openmittsu::protocol::ContactStatus const contactStatus = this->getContactStatus(it->id);
					if ((contactStatus == openmittsu::protocol::ContactStatus::DELETED) || (contactStatus == openmittsu::protocol::ContactStatus::KNOWN)) {
						if (getPublicKey(it->id) != it->publicKey) {
							throw openmittsu::exceptions::InternalErrorException() << "Can not create contact, inconsistent data: Contact " << it->id.toString() << " already exists with public key " << getPublicKey(it->id).toString() << ", which is different from the new public key " << it->publicKey.toString() << "!";
						}

						QSqlQuery query(m_database->getQueryObject());
						query.prepare(QStringLiteral("UPDATE `contacts` SET `verification` = :verificationStatus, `firstname` = :firstName, `lastname` = :lastName, `nick_name` = :nickName, `color` = :color WHERE `identity` = :identity;"));
						query.bindValue(QStringLiteral(":identity"), QVariant(it->id.toQString()));
						query.bindValue(QStringLiteral(":verificationStatus"), QVariant(openmittsu::protocol::ContactIdVerificationStatusHelper::toQString(it->verificationStatus)));
						query.bindValue(QStringLiteral(":firstName"), QVariant(it->firstName));
						query.bindValue(QStringLiteral(":lastName"), QVariant(it->lastName));
						query.bindValue(QStringLiteral(":nickName"), QVariant(it->nickName));
						query.bindValue(QStringLiteral(":color"), QVariant(it->color));

						if (!query.exec()) {
							throw openmittsu::exceptions::InternalErrorException() << "Could not update contact data for contact ID \"" << it->id.toString() << "\". Query error: " << query.lastError().text().toStdString();
						}

						m_database->announceContactChanged(it->id);
					} else {
						QSqlQuery query(m_database->getQueryObject());
						query.prepare(QStringLiteral("INSERT INTO `contacts` (`identity`, `publickey`, `verification`, `acid`, `tacid`, `firstname`, `lastname`, `nick_name`, `color`, `status`, `status_last_check`, `feature_level`, `feature_level_last_check`) VALUES "
													 "(:identity, :publickey, :verificationStatus, '', '', :firstName, :lastName, :nickName, :color, :status, -1, :featureLevel, -1);"));
						query.bindValue(QStringLiteral(":identity"), QVariant(it->id.toQString()));
						query.bindValue(QStringLiteral(":publickey"), QVariant(QString(it->publicKey.getPublicKey().toHex())));
						query.bindValue(QStringLiteral(":verificationStatus"), QVariant(openmittsu::protocol::ContactIdVerificationStatusHelper::toQString(it->verificationStatus)));
						query.bindValue(QStringLiteral(":firstName"), QVariant(it->firstName));
						query.bindValue(QStringLiteral(":lastName"), QVariant(it->lastName));
						query.bindValue(QStringLiteral(":nickName"), QVariant(it->nickName));
						query.bindValue(QStringLiteral(":color"), QVariant(it->color));
						query.bindValue(QStringLiteral(":status"), QVariant(openmittsu::protocol::AccountStatusHelper::toInt(openmittsu::protocol::AccountStatus::STATUS_UNKNOWN)));
						query.bindValue(QStringLiteral(":featureLevel"), QVariant(openmittsu::protocol::FeatureLevelHelper::toInt(openmittsu::protocol::FeatureLevel::LEVEL_UNKNOW)));

						if (!query.exec()) {
							throw openmittsu::exceptions::InternalErrorException() << "Could not insert contact into 'contacts'. Query error: " << query.lastError().text().toStdString();
						}

						m_database->announceContactChanged(it->id);
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

				m_database->transactionStart();
				for (; it != end; ++it) {
					setFields(it.key(), { {QStringLiteral("status"), openmittsu::protocol::AccountStatusHelper::toInt(it.value())}, {QStringLiteral("status_last_check"), timeNow} }, false);
				}
				m_database->transactionCommit();

				// TODO: Fixme. This might be broken
				m_database->announceContactChanged(m_database->getSelfContact());
			}

			void DatabaseContactAndGroupDataProvider::setFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels) {
				auto it = featureLevels.constBegin();
				auto end = featureLevels.constEnd();
				qint64 const timeNow = openmittsu::protocol::MessageTime::now().getMessageTimeMSecs();

				m_database->transactionStart();
				for (; it != end; ++it) {
					setFields(it.key(), { {QStringLiteral("feature_level"), openmittsu::protocol::FeatureLevelHelper::toInt(it.value())}, {QStringLiteral("feature_level_last_check"), timeNow} }, false);
				}
				m_database->transactionCommit();

				// TODO: Fixme. This might be broken
				m_database->announceContactChanged(m_database->getSelfContact());
			}

			QSet<openmittsu::protocol::ContactId> DatabaseContactAndGroupDataProvider::getKnownContacts() const {
				QSet<openmittsu::protocol::ContactId> result;

				QSqlQuery query(m_database->getQueryObject());
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
				QSqlQuery query(m_database->getQueryObject());
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
				QSqlQuery query(m_database->getQueryObject());
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

				QSqlQuery query(m_database->getQueryObject());
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

			QString DatabaseContactAndGroupDataProvider::buildNickname(QString const& nickname, QString const& firstName, QString const& lastName, openmittsu::protocol::ContactId const& contact) const {
				QString result = nickname;
				if (nickname.isNull() || nickname.isEmpty()) {
					if (!firstName.isEmpty() && !lastName.isEmpty()) {
						result = QString(firstName).append(" ").append(lastName);
					} else if (!firstName.isEmpty() && lastName.isEmpty()) {
						result = firstName;
					} else if (firstName.isEmpty() && !lastName.isEmpty()) {
						result = lastName;
					} else {
						result = contact.toQString();
					}
				}

				return result;
			}

			QHash<openmittsu::protocol::ContactId, QString> DatabaseContactAndGroupDataProvider::getKnownContactsWithNicknames(bool withSelfContactId) const {
				QHash<openmittsu::protocol::ContactId, QString> result;
				openmittsu::protocol::ContactId const selfContact = m_database->getSelfContact();

				QSqlQuery query(m_database->getQueryObject());
				query.prepare(QStringLiteral("SELECT `identity`, `firstname`, `lastname`, `nick_name` FROM `contacts`;"));

				if (query.exec() && query.isSelect()) {
					while (query.next()) {
						openmittsu::protocol::ContactId const contactId(query.value(QStringLiteral("identity")).toString());
						QString const nickname = buildNickname(query.value(QStringLiteral("nick_name")).toString(), query.value(QStringLiteral("firstname")).toString(), query.value(QStringLiteral("lastname")).toString(), contactId);

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

			std::shared_ptr<openmittsu::dataproviders::messages::ContactMessageCursor> DatabaseContactAndGroupDataProvider::getContactMessageCursor(openmittsu::protocol::ContactId const& contact) {
				return std::make_shared<openmittsu::database::internal::DatabaseContactMessageCursor>(m_database, contact);
			}

			QString DatabaseContactAndGroupDataProvider::getGroupDescription(QString const& memberField) const {
				QSet<openmittsu::protocol::ContactId> groupMembers = openmittsu::protocol::ContactIdList::fromString(memberField).getContactIds();
				QHash<openmittsu::protocol::ContactId, QString> nicknames = getNicknames(groupMembers);

				QString result;
				auto it = groupMembers.constBegin();
				auto end = groupMembers.constEnd();
				for (; it != end; ++it) {
					if (!result.isEmpty()) {
						result.append(QStringLiteral(", "));
					}

					auto const nicknameIt = nicknames.constFind(*it);
					if (nicknameIt == nicknames.constEnd()) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not build group description, missing contact nickname from set?!";
					}

					result.append(nicknameIt.value());
				}

				return result;
			}

			QHash<openmittsu::protocol::ContactId, QString> DatabaseContactAndGroupDataProvider::getNicknames(QSet<openmittsu::protocol::ContactId> const& contacts) const {
				QHash<openmittsu::protocol::ContactId, QString> result;
				if (contacts.size() == 0) {
					return result;
				}

				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `firstname`, `lastname`, `nick_name`, `identity` FROM `contacts`;"));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact nicknames query. Query error: " << query.lastError().text().toStdString();
				}

				while (query.next()) {
					openmittsu::protocol::ContactId const identity = openmittsu::protocol::ContactId(query.value(QStringLiteral("identity")).toString());
					if (contacts.contains(identity)) {
						QString const firstName = query.value(QStringLiteral("firstname")).toString();
						QString const lastName = query.value(QStringLiteral("lastname")).toString();
						QString const nickName = buildNickname(query.value(QStringLiteral("lastname")).toString(), firstName, lastName, identity);
						result.insert(identity, nickName);
					}
				}

				if (result.size() != contacts.size()) {
					throw openmittsu::exceptions::InternalErrorException() << "Fetching of contact nicknames failed, the result set has size " << result.size() << ", but the query set has size " << contacts.size() << "!";
				}

				return result;
			}

			openmittsu::database::ContactData DatabaseContactAndGroupDataProvider::getContactData(openmittsu::protocol::ContactId const& contact, bool fetchMessageCount) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `publickey`, `firstname`, `lastname`, `nick_name`, `status`, `verification`, `feature_level`, `color` FROM `contacts` WHERE `identity` = :identity;"));
				query.bindValue(QStringLiteral(":identity"), QVariant(contact.toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact data query for ID \"" << contact.toString() << "\". Query error: " << query.lastError().text().toStdString();
				} else if (!query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "No contact with identity \"" << contact.toString() << "\" exists, can not manipulate.";
				}

				ContactData result;
				result.publicKey = openmittsu::crypto::PublicKey::fromHexString(query.value(QStringLiteral("publickey")).toString());
				result.firstName = query.value(QStringLiteral("firstname")).toString();
				result.lastName = query.value(QStringLiteral("lastname")).toString();
				result.nickName = buildNickname(query.value(QStringLiteral("lastname")).toString(), result.firstName, result.lastName, contact);
				result.accountStatus = openmittsu::protocol::AccountStatusHelper::fromInt(query.value(QStringLiteral("status")).toInt());
				result.verificationStatus = openmittsu::protocol::ContactIdVerificationStatusHelper::fromQString(query.value(QStringLiteral("verification")).toString());
				result.featureLevel = openmittsu::protocol::FeatureLevelHelper::fromInt(query.value(QStringLiteral("feature_level")).toInt());
				result.color = query.value(QStringLiteral("color")).toInt();
				if (fetchMessageCount) {
					result.messageCount = openmittsu::database::internal::DatabaseContactMessage::getContactMessageCount(m_database, contact);
				} else {
					result.messageCount = -1;
				}

				return result;
			}

			QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> DatabaseContactAndGroupDataProvider::getContactDataAll(bool fetchMessageCount) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `identity`, `publickey`, `firstname`, `lastname`, `nick_name`, `status`, `verification`, `feature_level`, `color` FROM `contacts`;"));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute contact data enumeration query. Query error: " << query.lastError().text().toStdString();
				}

				QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> result;
				while (query.next()) {
					ContactData data;
					openmittsu::protocol::ContactId const identity = openmittsu::protocol::ContactId(query.value(QStringLiteral("identity")).toString());
					data.publicKey = openmittsu::crypto::PublicKey::fromHexString(query.value(QStringLiteral("publickey")).toString());
					data.firstName = query.value(QStringLiteral("firstname")).toString();
					data.lastName = query.value(QStringLiteral("lastname")).toString();
					data.nickName = buildNickname(query.value(QStringLiteral("lastname")).toString(), data.firstName, data.lastName, identity);
					data.accountStatus = openmittsu::protocol::AccountStatusHelper::fromInt(query.value(QStringLiteral("status")).toInt());
					data.verificationStatus = openmittsu::protocol::ContactIdVerificationStatusHelper::fromQString(query.value(QStringLiteral("verification")).toString());
					data.featureLevel = openmittsu::protocol::FeatureLevelHelper::fromInt(query.value(QStringLiteral("feature_level")).toInt());
					data.color = query.value(QStringLiteral("color")).toInt();
					if (fetchMessageCount) {
						data.messageCount = openmittsu::database::internal::DatabaseContactMessage::getContactMessageCount(m_database, identity);
					} else {
						data.messageCount = -1;
					}

					result.insert(identity, data);
				}

				return result;
			}

			openmittsu::database::GroupData DatabaseContactAndGroupDataProvider::getGroupData(openmittsu::protocol::GroupId const& group, bool withDescription) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `groupname`, `members`, `avatar_uuid`, `is_awaiting_sync` FROM `groups` WHERE `id` = :groupId AND `creator` = :groupCreator;"));
				query.bindValue(QStringLiteral(":groupId"), QVariant(group.groupIdWithoutOwnerToQString()));
				query.bindValue(QStringLiteral(":groupCreator"), QVariant(group.getOwner().toQString()));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group data query for table groups with group \"" << group.toString() << "\". Query error: " << query.lastError().text().toStdString();
				} else if (!query.next()) {
					throw openmittsu::exceptions::InternalErrorException() << "No group with group ID \"" << group.toString() << "\" exists, can not manipulate.";
				}

				GroupData result;
				result.title = query.value(QStringLiteral("groupname")).toString();

				QString const memberField = query.value(QStringLiteral("members")).toString();
				if (withDescription) {
					result.description = getGroupDescription(memberField);
				} else {
					result.description = QStringLiteral("");
				}

				result.members = openmittsu::protocol::ContactIdList::fromString(memberField).getContactIds();

				QVariant const avatar = queryField(group, QStringLiteral("avatar_uuid"));
				result.hasImage = !(avatar.isNull() || avatar.toString().isEmpty());

				if (result.hasImage) {
					result.image = openmittsu::database::MediaFileItem(openmittsu::database::MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE);
				} else {
					result.image = m_database->getMediaItem(avatar.toString());
				}

				result.isAwaitingSync = query.value(QStringLiteral("is_awaiting_sync")).toBool();
				result.messageCount = openmittsu::database::internal::DatabaseGroupMessage::getGroupMessageCount(m_database, group);

				return result;
			}

			QHash<openmittsu::protocol::GroupId, openmittsu::database::GroupData> DatabaseContactAndGroupDataProvider::getGroupDataAll(bool withDescription) const {
				QSqlQuery query(m_database->getQueryObject());

				query.prepare(QStringLiteral("SELECT `id`, `creator`, `groupname`, `members`, `avatar_uuid`, `is_awaiting_sync` FROM `groups`;"));

				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute group data enumeration query for table groups. Query error: " << query.lastError().text().toStdString();
				}

				QHash<openmittsu::protocol::GroupId, openmittsu::database::GroupData> result;
				while (query.next()) {
					GroupData groupData;
					openmittsu::protocol::ContactId const creator(query.value(QStringLiteral("creator")).toString());
					openmittsu::protocol::GroupId const group(creator, query.value(QStringLiteral("id")).toString());

					groupData.title = query.value(QStringLiteral("groupname")).toString();

					QString const memberField = query.value(QStringLiteral("members")).toString();
					if (withDescription) {
						groupData.description = getGroupDescription(memberField);
					} else {
						groupData.description = QStringLiteral("");
					}
					groupData.members = openmittsu::protocol::ContactIdList::fromString(memberField).getContactIds();

					QVariant const avatar = queryField(group, QStringLiteral("avatar_uuid"));
					groupData.hasImage = !(avatar.isNull() || avatar.toString().isEmpty());

					if (groupData.hasImage) {
						groupData.image = openmittsu::database::MediaFileItem(openmittsu::database::MediaFileItem::ItemStatus::UNAVAILABLE_NOT_IN_DATABASE);
					} else {
						groupData.image = m_database->getMediaItem(avatar.toString());
					}

					groupData.isAwaitingSync = query.value(QStringLiteral("is_awaiting_sync")).toBool();
					groupData.messageCount = openmittsu::database::internal::DatabaseGroupMessage::getGroupMessageCount(m_database, group);

					result.insert(group, groupData);
				}

				return result;
			}

		}
	}
}
