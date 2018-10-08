#ifndef OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTANDGROUPPOOL_H_
#define OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTANDGROUPPOOL_H_

#include <QHash>
#include <QMutex>

#include "src/database/DatabaseWrapper.h"
#include "src/dataproviders/MessageCenterWrapper.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

namespace openmittsu {
	namespace dataproviders {
		class BackedContact;
		class BackedGroup;

		class BackedContactAndGroupPool {
		public:
			std::shared_ptr<BackedContact> getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);
			std::shared_ptr<BackedGroup> getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::database::DatabaseWrapper const& database, openmittsu::dataproviders::MessageCenterWrapper const& messageCenter);

			static BackedContactAndGroupPool& getInstance();

			// delete copy and move constructors and assign operators
			BackedContactAndGroupPool(BackedContactAndGroupPool const&) = delete;             // Copy construct
			BackedContactAndGroupPool(BackedContactAndGroupPool&&) = delete;                  // Move construct
			BackedContactAndGroupPool& operator=(BackedContactAndGroupPool const&) = delete;  // Copy assign
			BackedContactAndGroupPool& operator=(BackedContactAndGroupPool &&) = delete;      // Move assign
		private:
			BackedContactAndGroupPool();
			virtual ~BackedContactAndGroupPool();

			QHash<openmittsu::protocol::ContactId, std::shared_ptr<openmittsu::dataproviders::BackedContact>> m_backedContacts;
			QHash<openmittsu::protocol::GroupId, std::shared_ptr<openmittsu::dataproviders::BackedGroup>> m_backedGroups;
			QMutex m_mutex;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_BACKEDCONTACTANDGROUPPOOL_H_
