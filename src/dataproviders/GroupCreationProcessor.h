#ifndef OPENMITTSU_DATAPROVIDERS_GROUPCREATIONPROCESSOR_H_
#define OPENMITTSU_DATAPROVIDERS_GROUPCREATIONPROCESSOR_H_

#include <QSet>
#include <QVariant>

#include "src/protocol/ContactId.h"

namespace openmittsu {
	namespace dataproviders {

		class GroupCreationProcessor {
		public:
			virtual ~GroupCreationProcessor() {}

			virtual bool createNewGroup(QSet<openmittsu::protocol::ContactId> const& groupMembers, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) = 0;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_GROUPCREATIONPROCESSOR_H_
