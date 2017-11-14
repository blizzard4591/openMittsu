#ifndef OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGE_H_
#define OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGE_H_

#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageTime.h"
#include "src/dataproviders/messages/Message.h"
#include "src/dataproviders/messages/ControlMessageType.h"
#include "src/dataproviders/messages/ControlMessageState.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class ControlMessage : public virtual Message {
			public:
				ControlMessage() {}
				virtual ~ControlMessage() {}

				virtual openmittsu::protocol::ContactId const& getContactId() const = 0;
				virtual ControlMessageType getControlMessageType() const = 0;
				virtual ControlMessageState getMessageState() const = 0;
				virtual void setMessageState(ControlMessageState const& messageState, openmittsu::protocol::MessageTime const& when) = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGE_H_
