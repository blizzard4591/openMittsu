#ifndef OPENMITTSU_CONTACTLISTWIDGETITEM_H_
#define OPENMITTSU_CONTACTLISTWIDGETITEM_H_

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QByteArray>

#include <cstdint>
#include "Contact.h"

class ContactListWidgetItem : public QListWidgetItem {
public:
	virtual ~ContactListWidgetItem() {}

	ContactListWidgetItem(Contact* contact, QListWidget * parent = 0, int type = Type);
	ContactListWidgetItem(Contact* contact, const QString & text, QListWidget * parent = 0, int type = Type);
	ContactListWidgetItem(Contact* contact, const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type);
	
	Contact* getContact() const;
private:
	Contact * const contact;
};

#endif // OPENMITTSU_CONTACTLISTWIDGETITEM_H_