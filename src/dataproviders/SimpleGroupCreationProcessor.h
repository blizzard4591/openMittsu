#ifndef OPENMITTSU_DATAPROVIDERS_SIMPLEGROUPCREATIONPROCESSOR_H_
#define OPENMITTSU_DATAPROVIDERS_SIMPLEGROUPCREATIONPROCESSOR_H_

#include <memory>

#include <QSet>

#include "src/dataproviders/GroupCreationProcessor.h"
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {

		class SimpleGroupCreationProcessor : public GroupCreationProcessor {
		public:
			explicit SimpleGroupCreationProcessor(std::weak_ptr<openmittsu::dataproviders::MessageCenter> const& messageCenter);
			virtual ~SimpleGroupCreationProcessor();

			virtual bool createNewGroup(QSet<openmittsu::protocol::ContactId> const& groupMembers, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) override;
		private:
			std::weak_ptr<openmittsu::dataproviders::MessageCenter> const m_messageCenter;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_SIMPLEGROUPCREATIONPROCESSOR_H_
