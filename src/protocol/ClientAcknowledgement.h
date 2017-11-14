#ifndef OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_
#define OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"

namespace openmittsu {
	namespace protocol {

		class ClientAcknowledgement {
		public:
			explicit ClientAcknowledgement(ContactId const& sender, MessageId const& messageId);
			virtual ~ClientAcknowledgement();

			virtual QByteArray toPacket() const;
		private:
			ContactId const sender;
			MessageId const messageId;
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_CLIENTACKNOWLEDGEMENT_H_
