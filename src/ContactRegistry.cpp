#include "ContactRegistry.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"
#include "IdentityHelper.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>

// Static Initializers
ContactRegistry* ContactRegistry::instance = nullptr;

ContactRegistry::ContactRegistry() : identitiesChangedSignalLocked(false), identityToIdentityContactHashMap(), identityToGroupContactHashMap(), emptyString("") {
	//
}

ContactRegistry* ContactRegistry::getInstance() {
	static QMutex mutex;
	if (instance == nullptr) {
		mutex.lock();

		if (instance == nullptr) {
			instance = new ContactRegistry();
		}

		mutex.unlock();
	}

	return instance;
}

IdentityContact* ContactRegistry::getSelfContact() const {
	return this->selfContact;
}

void ContactRegistry::setSelfContact(IdentityContact* newSelfContact) {
	this->selfContact = newSelfContact;
}

void ContactRegistry::fromFile(QString const& filename, bool dryRun) {
	if (!QFile::exists(filename)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file as it does not exist: %1").arg(filename).toStdString();
	}

	QFile inputFile(filename);
	if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file for reading: %1").arg(filename).toStdString();
	}

	accessMutex.lock();
	identitiesChangedSignalLocked = true;

	QRegExp commentRegExp("^\\s*#.*$", Qt::CaseInsensitive, QRegExp::RegExp2);
	QRegExp identityRegExp("^\\s*([A-Z0-9*][A-Z0-9]{7})\\s*:\\s*([a-fA-F0-9]{64})\\s*(?::\\s*(.*)\\s*)?$", Qt::CaseInsensitive, QRegExp::RegExp2);
	QRegExp groupRegExp("^\\s*([a-fA-F0-9]{16})\\s*:\\s*([A-Z0-9*][A-Z0-9]{7})\\s*:\\s*([A-Z0-9*][A-Z0-9]{7}(?:\\s*,\\s*[A-Z0-9*][A-Z0-9]{7})*)\\s*:\\s*(.*)\\s*$", Qt::CaseInsensitive, QRegExp::RegExp2);

	QTextStream in(&inputFile);
	in.setCodec("UTF-8"); // change the file codec to UTF-8.
	while (!in.atEnd()) {
		QString line = in.readLine().trimmed();
		if (line.isEmpty() || commentRegExp.exactMatch(line)) {
			continue;
		} else if (identityRegExp.exactMatch(line)) {
			if (!dryRun) {
				ContactId const contactId(identityRegExp.cap(1));
				PublicKey const publicKey = PublicKey::fromHexString(identityRegExp.cap(2));
				QString const trimmedNickname = identityRegExp.cap(3).trimmed();
				if (identityToIdentityContactHashMap.contains(contactId)) {
					LOGGER_DEBUG("Contact {} already exists in ContactRegistry, updating instead of recreating.", contactId.toString());
					IdentityContact* iContact = identityToIdentityContactHashMap.value(contactId);

					if (!(iContact->getPublicKey() == publicKey)) {
						LOGGER()->warn("Contact {} already exists in ContactRegistry, but with a different Public Key ({} vs. {})! Replacing contact.", contactId.toString(), iContact->getPublicKey().toString().toStdString(), publicKey.toString().toStdString());
						
						bool isSelfContact = false;
						if (selfContact == iContact) {
							selfContact = nullptr;
							isSelfContact = true;
						}
						
						disconnectContact(iContact);
						identityToIdentityContactHashMap.remove(contactId);
						delete iContact;

						iContact = new IdentityContact(contactId, publicKey);
						connectContact(iContact);
						identityToIdentityContactHashMap.insert(contactId, iContact);

						if (isSelfContact) {
							selfContact = iContact;
						}
					}
					if (!trimmedNickname.isEmpty()) {
						iContact->setNickname(trimmedNickname);
					}
				} else {
					IdentityContact* iContact = new IdentityContact(contactId, publicKey);
					if (!trimmedNickname.isEmpty()) {
						iContact->setNickname(trimmedNickname);
					}
					connectContact(iContact);
					identityToIdentityContactHashMap.insert(contactId, iContact);
				}
			}
		} else if (groupRegExp.exactMatch(line)) {
			if (!dryRun) {
				GroupId const groupId(IdentityHelper::identityStringToUint64(groupRegExp.cap(2)), IdentityHelper::groupIdByteArrayToUint64(QByteArray::fromHex(groupRegExp.cap(1).toLatin1())));
				QStringList ids = groupRegExp.cap(3).split(',', QString::SkipEmptyParts);
				if (ids.size() == 0) {
					identitiesChangedSignalLocked = false;
					accessMutex.unlock();
					throw IllegalArgumentException() << QString("Invalid or ill-formated line in contacts file \"%1\".\nProblematic line: %2").arg(filename).arg(line).toStdString();
				}

				QSet<ContactId> groupMembers;
				QStringList::const_iterator it = ids.constBegin();
				QStringList::const_iterator end = ids.constEnd();
				for (; it != end; ++it) {
					QString trimmedId(it->trimmed());
					ContactId const memberId(trimmedId);
					groupMembers.insert(memberId);
				}
				QString const groupName = groupRegExp.cap(4).trimmed();

				if (identityToGroupContactHashMap.contains(groupId)) {
					LOGGER_DEBUG("Group {} already exists in ContactRegistry, updating instead of recreating.", groupId.toString());

					GroupContact* gContact = identityToGroupContactHashMap.value(groupId);
					if (gContact->getGroupMembers() != groupMembers) {
						gContact->updateMembers(groupMembers);
					}

					if (!groupName.isEmpty()) {
						gContact->setGroupName(groupName);
					}
				} else {
					GroupContact* gContact = new GroupContact(groupId);
					if (!groupName.isEmpty()) {
						gContact->setGroupName(groupName);
					}

					gContact->updateMembers(groupMembers);

					connectContact(gContact);
					identityToGroupContactHashMap.insert(groupId, gContact);
				}
			}
		} else {
			identitiesChangedSignalLocked = false;
			accessMutex.unlock();
			throw IllegalArgumentException() << QString("Invalid or ill-formated line in contacts file \"%1\".\nProblematic line: %2").arg(filename).arg(line).toStdString();
		}
	}

	inputFile.close();

	identitiesChangedSignalLocked = false;
	accessMutex.unlock();

	if (!dryRun) {
		emit identitiesChanged();
	}
}

int ContactRegistry::getIdentityCount() const {
	return identityToIdentityContactHashMap.size();
}

int ContactRegistry::getGroupCount() const {
	return identityToGroupContactHashMap.size();
}

void ContactRegistry::toFile(QString const& filename) const {
	QFile outputFile(filename);
	if (!outputFile.open(QFile::WriteOnly | QFile::Text)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file for writing: %1").arg(filename).toStdString();
	}

	QTextStream outStream(&outputFile);
	outStream.setCodec("UTF-8"); // change the file codec to UTF-8.

	outStream << "# This is a comment line.\n";
	outStream << "# Format of this file: \n";
	outStream << "# IDENTITY : PUBKEY : Nickname\n";
	outStream << "# where \n";
	outStream << "# - IDENTITY is an eight character ID of the form [A-Z0-9*][A-Z0-9]{7} and stands for a users public id,\n";
	outStream << "# - PUBKEY is an 64 character key of the form [a-fA-F0-9]{64} and stands for a users 32-Byte long-term public key,\n";
	outStream << "# - Nickname is an optional screen-name for the given identity.\n";
	outStream << "# GROUPID : GROUPOWNER : IDENTITY, IDENTITY, IDENTITY : Group Name\n";
	outStream << "# where \n";
	outStream << "# - IDENTITY is an eight character ID of the form [A-Z0-9*][A-Z0-9]{7} and stands for a users public id,\n";
	outStream << "# - GROUPID is an 16 character key of the form [a-fA-F0-9]{16} and stands for a groups unique identifier,\n";
	outStream << "# - GROUPOWNER is an IDENTITY and stands for a groups creator and owner,\n";
	outStream << "# - Group Name is the displayed title of the group.\n";

	accessMutex.lock();
	// Convert identityToIdentityContactHashMap into a QMap for sorted output.
	QMap<ContactId, IdentityContact*> sortedContactMap;
	QHash<ContactId, IdentityContact*>::const_iterator itContacts = identityToIdentityContactHashMap.constBegin();
	QHash<ContactId, IdentityContact*>::const_iterator endContacts = identityToIdentityContactHashMap.constEnd();
	for (; itContacts != endContacts; ++itContacts) {
		sortedContactMap.insert(itContacts.key(), itContacts.value());
	}

	QMap<ContactId, IdentityContact*>::const_iterator itSortedContacts = sortedContactMap.constBegin();
	QMap<ContactId, IdentityContact*>::const_iterator endSortedContacts = sortedContactMap.constEnd();
	for (; itSortedContacts != endSortedContacts; ++itSortedContacts) {
		IdentityContact* ic = itSortedContacts.value();
		QString nickname = ic->getNickname();
		if (nickname.isNull() || nickname.isEmpty()) {
			nickname = "";
		}
		outStream << ic->getContactId().toQString() << " : " << QString(ic->getPublicKey().getPublicKey().toHex()) << " : " << nickname << "\n";
	}

	// Convert identityToGroupContactHashMap into a QMap for sorted output.
	QMap<GroupId, GroupContact*> sortedGroupMap;
	QHash<GroupId, GroupContact*>::const_iterator itGroups = identityToGroupContactHashMap.constBegin();
	QHash<GroupId, GroupContact*>::const_iterator endGroups = identityToGroupContactHashMap.constEnd();
	for (; itGroups != endGroups; ++itGroups) {
		sortedGroupMap.insert(itGroups.key(), itGroups.value());
	}

	QMap<GroupId, GroupContact*>::const_iterator itSortedGroups = sortedGroupMap.constBegin();
	QMap<GroupId, GroupContact*>::const_iterator endSortedGroups = sortedGroupMap.constEnd();
	for (; itSortedGroups != endSortedGroups; ++itSortedGroups) {
		GroupContact* gc = itSortedGroups.value();
		QSet<ContactId> const& members = gc->getGroupMembers();
		QSet<ContactId>::const_iterator membersIt = members.constBegin();
		QSet<ContactId>::const_iterator membersEnd = members.constEnd();
		QStringList memberIds;
		for (; membersIt != membersEnd; ++membersIt) {
			memberIds.append(membersIt->toQString());
		}
		memberIds.sort(Qt::CaseInsensitive);

		outStream << gc->getGroupId().toContactFileFormat() << " : " << memberIds.join(',') << " : " << gc->getContactName() << "\n";
	}

	accessMutex.unlock();

	outputFile.close();
}

bool ContactRegistry::hasIdentity(ContactId const& identity) const {
	accessMutex.lock();
	bool result = identityToIdentityContactHashMap.contains(identity);
	accessMutex.unlock();

	return result;
}

bool ContactRegistry::hasGroup(GroupId const& identity) const {
	accessMutex.lock();
	bool result = identityToGroupContactHashMap.contains(identity);
	accessMutex.unlock();

	return result;
}

PublicKey const& ContactRegistry::getPublicKey(ContactId const& identity) const {
	accessMutex.lock();
	PublicKey const& result = identityToIdentityContactHashMap.find(identity).value()->getPublicKey();
	accessMutex.unlock();

	return result;
}

QHash<GroupId, std::pair<QSet<ContactId>, QString>> ContactRegistry::getKnownGroupsWithMembersAndTitles() const {
	QHash<GroupId, std::pair<QSet<ContactId>, QString>> result;

	QHash<GroupId, GroupContact*>::const_iterator identityToGroupContactHashMapIt = identityToGroupContactHashMap.constBegin();
	QHash<GroupId, GroupContact*>::const_iterator identityToGroupContactHashMapEnd = identityToGroupContactHashMap.constEnd();
	for (; identityToGroupContactHashMapIt != identityToGroupContactHashMapEnd; ++identityToGroupContactHashMapIt) {
		result.insert(identityToGroupContactHashMapIt.key(), std::make_pair(identityToGroupContactHashMapIt.value()->getGroupMembers(), identityToGroupContactHashMapIt.value()->getContactName()));
	}
	return result;
}

QHash<ContactId, PublicKey> ContactRegistry::getKnownIdentitiesWithPublicKeys() const {
	QHash<ContactId, PublicKey> result;

	QHash<ContactId, IdentityContact*>::const_iterator identityToIdentityContactHashMapIt = identityToIdentityContactHashMap.constBegin();
	QHash<ContactId, IdentityContact*>::const_iterator identityToIdentityContactHashMapEnd = identityToIdentityContactHashMap.constEnd();
	for (; identityToIdentityContactHashMapIt != identityToIdentityContactHashMapEnd; ++identityToIdentityContactHashMapIt) {
		result.insert(identityToIdentityContactHashMapIt.key(), identityToIdentityContactHashMapIt.value()->getPublicKey());
	}
	return result;
}

QString ContactRegistry::getNickname(ContactId const& identity) const {
	if (hasIdentity(identity)) {
		accessMutex.lock();
		QString result = identityToIdentityContactHashMap.find(identity).value()->getContactName();
		accessMutex.unlock();
		return result;
	}

	return emptyString;
}

QString ContactRegistry::getNickname(GroupId const& group) const {
	if (hasGroup(group)) {
		accessMutex.lock();
		QString result = identityToGroupContactHashMap.find(group).value()->getContactName();
		accessMutex.unlock();
		return result;
	}

	return emptyString;
}

void ContactRegistry::addIdentity(IdentityContact* contact) {
	accessMutex.lock();
	
	identityToIdentityContactHashMap.insert(contact->getContactId(), contact);
	connectContact(contact);

	accessMutex.unlock();
	emit identitiesChanged();
}

void ContactRegistry::addGroup(GroupContact* contact) {
	accessMutex.lock();

	identityToGroupContactHashMap.insert(contact->getGroupId(), contact);
	connectContact(contact);

	accessMutex.unlock();
	emit identitiesChanged();
}

IdentityContact* ContactRegistry::getIdentity(ContactId const& identity) const {
	QHash<ContactId, IdentityContact*>::const_iterator itContacts(identityToIdentityContactHashMap.find(identity));
	if (itContacts != identityToIdentityContactHashMap.constEnd()) {
		return itContacts.value();
	}
	return nullptr;
}

GroupContact* ContactRegistry::getGroup(GroupId const& identity) const {
	QHash<GroupId, GroupContact*>::const_iterator itGroups(identityToGroupContactHashMap.find(identity));
	if (itGroups != identityToGroupContactHashMap.constEnd()) {
		return itGroups.value();
	}
	return nullptr;
}

QList<ContactId> ContactRegistry::getIdentities() const {
	return identityToIdentityContactHashMap.keys();
}

QList<GroupId> ContactRegistry::getGroups() const {
	return identityToGroupContactHashMap.keys();
}

void ContactRegistry::connectContact(Contact* contact) {
	OPENMITTSU_CONNECT(contact, dataChanged(), this, onContactDataChanged());
}

void ContactRegistry::disconnectContact(Contact* contact) {
	OPENMITTSU_DISCONNECT(contact, dataChanged(), this, onContactDataChanged());
}

void ContactRegistry::onContactDataChanged() {
	if (!identitiesChangedSignalLocked) {
		LOGGER_DEBUG("Ignoring onContactDataChanged() since the internal lock is set.");
		emit identitiesChanged();
	}
}