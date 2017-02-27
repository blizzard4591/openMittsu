#include "IdentityContact.h"

#include "IdentityHelper.h"

#include "exceptions/InternalErrorException.h"
#include "exceptions/NotConnectedException.h"

IdentityContact::IdentityContact(ContactId const& contactIdentity, PublicKey const& contactPublicKey) : contactId(contactIdentity), contactPublicKey(contactPublicKey), status(ContactIdStatus::STATUS_UNKNOWN), nickname() {
	//
}

IdentityContact::~IdentityContact() {
	//
}

QString IdentityContact::getContactName() const {
	if (nickname.isNull() || nickname.isEmpty()) {
		return contactId.toQString();
	} else {
		return QString("%1 (%2)").arg(nickname).arg(contactId.toQString());
	}
}

QString IdentityContact::getContactDescription() const {
	return QString("");
}

void IdentityContact::setNickname(QString const& nickname) {
	this->nickname = nickname;
	emit dataChanged();
}

QString const& IdentityContact::getNickname() const {
	return nickname;
}

PublicKey const& IdentityContact::getPublicKey() const {
	return contactPublicKey;
}

ContactId const& IdentityContact::getContactId() const {
	return contactId;
}

Contact::ContactType IdentityContact::getContactType() const {
	return ContactType::CONTACT_IDENTITY;
}

ContactIdStatus const& IdentityContact::getStatus() const {
	return status;
}

void IdentityContact::setStatus(ContactIdStatus const& newStatus) {
	status = newStatus;
}
