#ifndef OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QByteArray>

#include <cstdint>
#include "Contact.h"

class ContactListWidgetItem : public QListWidgetItem {
public:
	virtual ~ContactListWidgetItem() {}
	ContactListWidgetItem(Contact* contact, const QString& text, QListWidget* parent = nullptr, int type = Type);
	
	Contact* getContact() const;

	virtual bool operator<(QListWidgetItem const& other) const override;
private:
	Contact * const contact;
};

#endif // OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_
