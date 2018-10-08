#include "src/dataproviders/SimpleGroupCreationProcessor.h"

namespace openmittsu {
	namespace dataproviders {

		SimpleGroupCreationProcessor::SimpleGroupCreationProcessor(openmittsu::dataproviders::MessageCenterWrapper const& messageCenter) : GroupCreationProcessor(), 
			m_messageCenter(messageCenter) {
			//
		}

		SimpleGroupCreationProcessor::~SimpleGroupCreationProcessor() {
			//
		}

		bool SimpleGroupCreationProcessor::createNewGroup(QSet<openmittsu::protocol::ContactId> const& groupMembers, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) {
			if (!m_messageCenter.hasMessageCenter()) {
				return false;
			}

			return m_messageCenter.createNewGroupAndInformMembers(groupMembers, addSelfContact, groupTitle, groupImage);
		}

	}
}
