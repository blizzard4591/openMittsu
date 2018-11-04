#include "src/widgets/GroupListWidgetItem.h"

#include "src/widgets/ContactListWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		GroupListWidgetItem::GroupListWidgetItem(openmittsu::protocol::GroupId const& group, bool sortById, const QString & text, QListWidget * parent, int type) : QListWidgetItem(text, parent, type), m_groupId(group), m_sortById(sortById) {
			//
		}

		openmittsu::protocol::GroupId const& GroupListWidgetItem::getGroupId() const {
			return m_groupId;
		}

		bool GroupListWidgetItem::operator <(QListWidgetItem const& other) const {
			ContactListWidgetItem const*const otherItemContact = dynamic_cast<ContactListWidgetItem const*>(&other);
			GroupListWidgetItem const*const otherItemGroup = dynamic_cast<GroupListWidgetItem const*>(&other);
			if (otherItemContact != nullptr) {
				return false; // contact before group
			} else if (otherItemGroup != nullptr) {
				if (m_sortById) {
					return (getGroupId() < otherItemGroup->getGroupId());
				} else {
					return (this->text().localeAwareCompare(other.text()) < 0);
				}
			} else {
				return (this->text().localeAwareCompare(other.text()) < 0);
			}
		}

	}
}

