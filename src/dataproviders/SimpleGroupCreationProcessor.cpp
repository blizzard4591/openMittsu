#include "src/dataproviders/SimpleGroupCreationProcessor.h"

namespace openmittsu {
	namespace dataproviders {

		SimpleGroupCreationProcessor::SimpleGroupCreationProcessor(std::weak_ptr<openmittsu::dataproviders::MessageCenter> const& messageCenter) : GroupCreationProcessor(), 
			m_messageCenter(messageCenter) {
			//
		}

		SimpleGroupCreationProcessor::~SimpleGroupCreationProcessor() {
			//
		}

		bool SimpleGroupCreationProcessor::createNewGroup(QSet<openmittsu::protocol::ContactId> const& groupMembers, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) {
			auto mc = m_messageCenter.lock();
			if (!mc) {
				return false;
			}

			return mc->createNewGroupAndInformMembers(groupMembers, addSelfContact, groupTitle, groupImage);
		}

	}
}
