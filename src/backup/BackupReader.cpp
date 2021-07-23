#include "src/backup/BackupReader.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QRegularExpression>

#include "src/backup/FileReader.h"

#include "src/backup/IdentityBackup.h"
#include "src/backup/IdentityBackupObject.h"
#include "src/backup/ContactBackupObject.h"
#include "src/backup/GroupBackupObject.h"
#include "src/backup/ContactMessageBackupObject.h"
#include "src/backup/GroupMessageBackupObject.h"

#include "src/backup/ContactMediaItemBackupObject.h"
#include "src/backup/ContactThumbnailMediaItemBackupObject.h"
#include "src/backup/GroupMediaItemBackupObject.h"
#include "src/backup/GroupThumbnailMediaItemBackupObject.h"
#include "src/backup/GroupAvatarMediaItemBackupObject.h"

#include "src/database/SimpleDatabase.h"

#include "src/exceptions/InvalidInputException.h"
#include "src/utility/Logging.h"

#include <chrono>

namespace openmittsu {
	namespace backup {

		BackupReader::BackupReader(QDir const& backupFilePath, QString const& backupPassword, QString const& databaseFilename, QDir const& mediaStorageLocation, QString const& databasePassword) : m_backupFilePath(backupFilePath), m_backupPassword(backupPassword), m_databaseFilename(databaseFilename), m_mediaStorageLocation(mediaStorageLocation), m_databasePassword(databasePassword) {
			//
		}

		BackupReader::~BackupReader() {
			//
		}

		void BackupReader::run() {
			emit progressUpdated(0);

			try {
				QList<ContactBackupObject> contacts;
				QList<GroupBackupObject> groups;
				QList<ContactMessageBackupObject> contactMessages;
				QList<GroupMessageBackupObject> groupMessages;
				QList<ContactMediaItemBackupObject> contactMediaFiles;
				QList<GroupMediaItemBackupObject> groupMediaFiles;

				QString const identityBackupString = IdentityBackupObject::fromFile(m_backupFilePath).getBackupString();
				// This will throw if the password/backup is invalid.
				IdentityBackup const identityBackup = IdentityBackup::fromBackupString(identityBackupString, m_backupPassword);

				std::shared_ptr<openmittsu::database::SimpleDatabase> const database = std::make_shared<openmittsu::database::SimpleDatabase>(m_databaseFilename, identityBackup.getClientContactId(), identityBackup.getClientLongTermKeyPair(), m_databasePassword, m_mediaStorageLocation);

				{
					LOGGER_DEBUG("Will now read the contacts.csv...");
					FileReader<ContactBackupObject> fileReader(m_backupFilePath, QStringLiteral("contacts.csv"));
					QSet<openmittsu::protocol::ContactId> knownContacts;
					QVector<openmittsu::database::NewContactData> newContacts;

					while (fileReader.hasNext()) {
						ContactBackupObject const cbo = fileReader.getNext();
						if (!knownContacts.contains(cbo.getContactId())) {
							knownContacts.insert(cbo.getContactId());
							openmittsu::database::NewContactData newContact(cbo.getContactId(), cbo.getPublicKey(), cbo.getVerificationStatus(), cbo.getFirstName(), cbo.getLastName(), cbo.getNickName(), cbo.getColor());
							newContacts.append(newContact);
						} else {
							LOGGER()->warn("Contact {} is already in database, this should not happen!", cbo.getContactId().toString());
						}
						contacts.append(cbo);
					}
					database->storeNewContact(newContacts);

					LOGGER()->info("Parsed {} contacts from file.", contacts.size());
					emit progressUpdated(5);
				}

				{
					LOGGER_DEBUG("Will now read the groups.csv...");
					FileReader<GroupBackupObject> fileReader(m_backupFilePath, QStringLiteral("groups.csv"));
					QSet<openmittsu::protocol::GroupId> knownGroups;
					QVector<openmittsu::database::NewGroupData> newGroups;

					while (fileReader.hasNext()) {
						GroupBackupObject const gbo = fileReader.getNext();
						if (!knownGroups.contains(gbo.getGroupId())) {
							knownGroups.insert(gbo.getGroupId());
							openmittsu::database::NewGroupData newGroup(gbo.getGroupId(), gbo.getName(), gbo.getCreatedAt(), gbo.getMembers(), gbo.getIsDeleted(), false);
							newGroups.append(newGroup);
						} else {
							LOGGER()->warn("Group {} is already in database, this should not happen!", gbo.getGroupId().toString());
						}
						groups.append(gbo);
					}
					database->storeNewGroup(newGroups);

					LOGGER()->info("Parsed {} groups from file.", groups.size());
					emit progressUpdated(10);
				}

				{
					LOGGER_DEBUG("Will now read the contact message files...");
					QHash<openmittsu::protocol::ContactId, QString> const contactMessageFiles = ContactMessageBackupObject::getContactMessageFiles(m_backupFilePath);
					LOGGER()->info("Found {} contacts message files.", contactMessageFiles.size());

					auto it = contactMessageFiles.constBegin();
					auto end = contactMessageFiles.constEnd();

					for (; it != end; ++it) {
						LOGGER()->info("Will now read contact message file {}.", it.value().toStdString());
						FileReader<ContactMessageBackupObject> fileReader(m_backupFilePath, it.value());
						while (fileReader.hasNext()) {
							ContactMessageBackupObject const cmbo = fileReader.getNext();
							contactMessages.append(cmbo);
						}
					}
					LOGGER()->info("Parsed {} contact messages from files.", contactMessages.size());
					emit progressUpdated(25);

					database->storeContactMessagesFromBackup(contactMessages);
					LOGGER()->info("Database now contains {} contact messages.", database->getContactMessageCount());
					emit progressUpdated(40);
				}

				{
					LOGGER_DEBUG("Will now read the group message files...");
					QHash<openmittsu::protocol::GroupId, QString> const groupMessageFiles = GroupMessageBackupObject::getGroupMessageFiles(m_backupFilePath);
					LOGGER()->info("Found {} group message files.", groupMessageFiles.size());

					auto it = groupMessageFiles.constBegin();
					auto end = groupMessageFiles.constEnd();
					for (; it != end; ++it) {
						LOGGER()->info("Will now read group message file {}.", it.value().toStdString());
						FileReader<GroupMessageBackupObject> fileReader(m_backupFilePath, it.value());
						while (fileReader.hasNext()) {
							GroupMessageBackupObject const gmbo = fileReader.getNext();
							groupMessages.append(gmbo);
						}
					}
					LOGGER()->info("Parsed {} group messages from files.", groupMessages.size());
					emit progressUpdated(55);

					database->storeGroupMessagesFromBackup(groupMessages);
					LOGGER()->info("Database now contains {} group messages.", database->getGroupMessageCount());
					emit progressUpdated(70);
				}


				//
				// Media Items
				//
				{
					LOGGER_DEBUG("Will now read the contact media files...");
					QHash<QString, QString> const contactMediaItems = ContactMediaItemBackupObject::getContactMediaFiles(m_backupFilePath);
					LOGGER()->info("Found {} contact media files.", contactMediaItems.size());

					auto it = contactMediaItems.constBegin();
					auto end = contactMediaItems.constEnd();
					for (; it != end; ++it) {
						LOGGER()->info("Will now read contact media file {}.", it.value().toStdString());
						ContactMediaItemBackupObject const cmibo = ContactMediaItemBackupObject::fromFile(m_backupFilePath, it.value());
						contactMediaFiles.append(cmibo);
					}
					LOGGER()->info("Parsed {} contact media files.", contactMediaFiles.size());
					emit progressUpdated(77);

					database->storeContactMediaItemsFromBackup(contactMediaFiles);
					LOGGER()->info("Database now contains {} media items.", database->getMediaItemCount());
					emit progressUpdated(85);
				}

				{
					LOGGER_DEBUG("Will now read the group media files...");
					QHash<QString, QString> const groupMediaItems = GroupMediaItemBackupObject::getGroupMediaFiles(m_backupFilePath);
					LOGGER()->info("Found {} group media files.", groupMediaItems.size());

					auto it = groupMediaItems.constBegin();
					auto end = groupMediaItems.constEnd();
					for (; it != end; ++it) {
						LOGGER()->info("Will now read group media file {}.", it.value().toStdString());
						groupMediaFiles.append(GroupMediaItemBackupObject::fromFile(m_backupFilePath, it.value()));
					}
					LOGGER()->info("Parsed {} group media files.", groupMediaFiles.size());
					emit progressUpdated(92);

					database->storeGroupMediaItemsFromBackup(groupMediaFiles);
					LOGGER()->info("Database now contains {} media items.", database->getMediaItemCount());
					emit progressUpdated(100);
				}
				emit finished(false, "");
			} catch (openmittsu::exceptions::BaseException& be) {
				emit finished(true, tr("An error occured while importing the data backup into database.\nProblem: %1").arg(be.what()));
			} catch (...) {
				emit finished(true, tr("An unexpected error occured while importing the data backup into database.\nUnknown Problem."));
			}
		}

	}
}
