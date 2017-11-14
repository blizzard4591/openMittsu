#ifndef OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"

namespace openmittsu {
	namespace protocol {

		class UniqueMessageIdGenerator {
		public:
			virtual ~UniqueMessageIdGenerator() {}

			virtual MessageId getNextUniqueMessageId(ContactId const& receiver) = 0;
			virtual MessageId getNextUniqueMessageId(GroupId const& receiver) = 0;
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_UNIQUEMESSAGEIDGENERATOR_H_
