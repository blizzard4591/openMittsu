#include "src/dataproviders/MessageCenterWrapperFactory.h"

#include "src/dataproviders/MessageCenterPointerAuthority.h"
#include "src/dataproviders/MessageCenterWrapper.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace dataproviders {

		MessageCenterWrapperFactory::MessageCenterWrapperFactory(MessageCenterPointerAuthority const& messageCenterPointerAuthority) : m_messageCenterPointerAuthority(messageCenterPointerAuthority) {
			//
		}

		MessageCenterWrapperFactory::~MessageCenterWrapperFactory() {
			//
		}

		MessageCenterWrapper MessageCenterWrapperFactory::getMessageCenterWrapper() const {
			return MessageCenterWrapper(m_messageCenterPointerAuthority);
		}

	}
}
