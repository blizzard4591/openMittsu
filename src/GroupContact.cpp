#include "GroupContact.h"

#include "exceptions/InternalErrorException.h"
#include "exceptions/NotConnectedException.h"
#include "ContactRegistry.h"
#include "IdentityHelper.h"

#include <QTimer>
#include <QDateTime>

GroupContact::GroupContact(GroupId const& groupId) : groupId(groupId), groupName(IdentityHelper::uint64ToGroupIdByteArray(groupId.getGroupId()).toHex()) {
	//
}

GroupContact::~GroupContact() {
	//
}

QString GroupContact::getContactName() const {
	if (groupName.isNull() || groupName.isEmpty()) {
		return groupId.toQString();
	}
	return groupName;
}

ContactId const& GroupContact::getGroupOwner() const {
	return groupId.getOwner();
}

QString GroupContact::getContactDescription() const {
	QString result;
	QSet<ContactId>::const_iterator it = groupMembers.constBegin();
	QSet<ContactId>::const_iterator end = groupMembers.constEnd();
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();
	for (; it != end; ++it) {
		if (!result.isEmpty()) {
			result.append(", ");
		}
		
		result.append(contactRegistry->getNickname(*it));
	}
	return result;
}

void GroupContact::setGroupName(QString const& groupName) {
	this->groupName = groupName;
	emit dataChanged();
}

Contact::ContactType GroupContact::getContactType() const {
	return ContactType::CONTACT_GROUP;
}

bool GroupContact::hasMember(ContactId const& identity) const {
	return groupMembers.contains(identity);
}

GroupId const& GroupContact::getGroupId() const {
	return groupId;
}

void GroupContact::addMember(ContactId const& identity) {
	groupMembers.insert(identity);
	emit dataChanged();
}

void GroupContact::addMember(QSet<ContactId> const& newMemberSet) {
	QSet<ContactId>::const_iterator it = newMemberSet.constBegin();
	QSet<ContactId>::const_iterator end = newMemberSet.constEnd();
	for (; it != end; ++it) {
		groupMembers.insert(*it);
	}
	emit dataChanged();
}

void GroupContact::removeMember(ContactId const& identity) {
	groupMembers.remove(identity);
	emit dataChanged();
}

QSet<ContactId> const& GroupContact::getGroupMembers() const {
	return groupMembers;
}

void GroupContact::updateMembers(QSet<ContactId> const& newMemberSet) {
	this->groupMembers = newMemberSet;
	if (!groupMembers.contains(groupId.getOwner())) {
		groupMembers.insert(groupId.getOwner());
	}

	emit dataChanged();
}

quint64 GroupContact::generateRandomGroupId() {
	quint64 id = 0;
	randombytes_buf(&id, sizeof(quint64));
	
	return id;
}