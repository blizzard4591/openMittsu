#include "widgets/ContactListWidgetItem.h"

#include "IdentityContact.h"
#include "GroupContact.h"

ContactListWidgetItem::ContactListWidgetItem(Contact* contact, const QString & text, QListWidget * parent, int type) : QListWidgetItem(text, parent, type), contact(contact) {
	//
}

Contact* ContactListWidgetItem::getContact() const {
	return contact;
}

bool ContactListWidgetItem::operator <(QListWidgetItem const& other) const {
	ContactListWidgetItem const*const otherPtr = dynamic_cast<ContactListWidgetItem const*>(&other);
	if (otherPtr == nullptr) {
		return true;
	}

	Contact::ContactType const ourType = contact->getContactType();
	if ((ourType == Contact::ContactType::CONTACT_IDENTITY) && (otherPtr->contact->getContactType() == Contact::ContactType::CONTACT_GROUP)) {
		return true;
	} else if ((ourType == Contact::ContactType::CONTACT_GROUP) && (otherPtr->contact->getContactType() == Contact::ContactType::CONTACT_IDENTITY)) {
		return false;
	} else if (ourType == Contact::ContactType::CONTACT_IDENTITY) {
		IdentityContact const * const a = dynamic_cast<IdentityContact const*>(contact);
		IdentityContact const * const b = dynamic_cast<IdentityContact const*>(otherPtr->contact);
		return (a->getContactId().operator<(b->getContactId()));
	} else {
		return (this->text().localeAwareCompare(other.text()) < 0);
	}
}
