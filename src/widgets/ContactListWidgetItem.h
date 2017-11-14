#ifndef OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QByteArray>

#include <cstdint>

#include "src/protocol/ContactId.h"

class ContactListWidgetItem : public QListWidgetItem {
public:
	virtual ~ContactListWidgetItem() {}
	ContactListWidgetItem(openmittsu::protocol::ContactId const& contactId, bool sortById, const QString& text, QListWidget* parent = nullptr, int type = Type);
	
	openmittsu::protocol::ContactId const& getContactId() const;

	virtual bool operator<(QListWidgetItem const& other) const override;
private:
	openmittsu::protocol::ContactId const m_contactId;
	bool const m_sortById;
};

#endif // OPENMITTSU_WIDGETS_CONTACTLISTWIDGETITEM_H_
