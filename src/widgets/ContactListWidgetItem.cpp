#include "src/widgets/ContactListWidgetItem.h"

#include "src/widgets/GroupListWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		ContactListWidgetItem::ContactListWidgetItem(openmittsu::protocol::ContactId const& contactId, bool sortById, QString const& text, QListWidget* parent, int type) : QListWidgetItem(text, parent, type), m_contactId(contactId), m_sortById(sortById) {
			//
		}

		openmittsu::protocol::ContactId const& ContactListWidgetItem::getContactId() const {
			return m_contactId;
		}

		bool ContactListWidgetItem::operator <(QListWidgetItem const& other) const {
			ContactListWidgetItem const*const otherItemContact = dynamic_cast<ContactListWidgetItem const*>(&other);
			GroupListWidgetItem const*const otherItemGroup = dynamic_cast<GroupListWidgetItem const*>(&other);
			if (otherItemContact != nullptr) {
				if (m_sortById) {
					return (getContactId() < otherItemContact->getContactId());
				} else {
					return (this->text().localeAwareCompare(other.text()) < 0);
				}
			} else if (otherItemGroup != nullptr) {
				return true; // contact before group
			} else {
				return (this->text().localeAwareCompare(other.text()) < 0);
			}
		}

	}
}
