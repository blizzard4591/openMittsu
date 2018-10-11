#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPERFACTORY_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPERFACTORY_H_

#include <memory>

#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenterPointerAuthority;
		class MessageCenterWrapper;

		class MessageCenterWrapperFactory {
		public:
			MessageCenterWrapperFactory();
			MessageCenterWrapperFactory(MessageCenterPointerAuthority const* messageCenterPointerAuthority);
			virtual ~MessageCenterWrapperFactory();

			MessageCenterWrapper getMessageCenterWrapper() const;
		private:
			MessageCenterPointerAuthority const* m_messageCenterPointerAuthority;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPERFACTORY_H_
