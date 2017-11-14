#ifndef OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGECURSOR_H_
#define OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGECURSOR_H_

#include <memory>

#include "src/dataproviders/messages/ControlMessage.h"
#include "src/dataproviders/messages/MessageCursor.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ControlMessageCursor : public virtual MessageCursor {
			public:
				virtual ~ControlMessageCursor() {}

				virtual std::shared_ptr<ControlMessage> getMessage() = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGECURSOR_H_
