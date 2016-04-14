#include "ContactListWidgetItem.h"

ContactListWidgetItem::ContactListWidgetItem(Contact* contact, QListWidget * parent, int type) : QListWidgetItem(parent, type), contact(contact) {
	//
}

ContactListWidgetItem::ContactListWidgetItem(Contact* contact, const QString & text, QListWidget * parent, int type) : QListWidgetItem(text, parent, type), contact(contact) {
	//
}

ContactListWidgetItem::ContactListWidgetItem(Contact* contact, const QIcon & icon, const QString & text, QListWidget * parent, int type) : QListWidgetItem(icon, text, parent, type), contact(contact) {
	//
}

Contact* ContactListWidgetItem::getContact() const {
	return contact;
}