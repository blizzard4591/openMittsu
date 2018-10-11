#include "src/dataproviders/BackedContactAndGroupPool.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedGroup.h"

#include <QMutexLocker>

namespace openmittsu {
	namespace dataproviders {

		std::shared_ptr<BackedContact> BackedContactAndGroupPool::getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) {
			QMutexLocker lock(&m_mutex);
			auto it = m_backedContacts.constFind(contact);
			if (it != m_backedContacts.constEnd()) {
				return it.value();
			} else {
				std::shared_ptr<BackedContact> backedContact = std::make_shared<BackedContact>(contact, database, messageCenter, *this);
				m_backedContacts.insert(contact, backedContact);
				return backedContact;
			}
		}

		std::shared_ptr<BackedGroup> BackedContactAndGroupPool::getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) {
			QMutexLocker lock(&m_mutex);
			auto it = m_backedGroups.constFind(group);
			if (it != m_backedGroups.constEnd()) {
				return it.value();
			} else {
				std::shared_ptr<BackedGroup> backedGroup = std::make_shared<BackedGroup>(group, database, messageCenter, *this);
				m_backedGroups.insert(group, backedGroup);
				return backedGroup;
			}
		}

		BackedContactAndGroupPool& BackedContactAndGroupPool::getInstance() {
			static BackedContactAndGroupPool instance;
			return instance;
		}

		BackedContactAndGroupPool::BackedContactAndGroupPool() : m_backedContacts(), m_backedGroups(), m_mutex() {
			//
		}

		BackedContactAndGroupPool::~BackedContactAndGroupPool() {
			//
		}

	}
}