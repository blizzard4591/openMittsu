#ifndef OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_

#include "src/protocol/GroupId.h"
#include "src/messages/PreliminaryMessageHeader.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			class PreliminaryGroupMessageHeader : public PreliminaryMessageHeader {
			public:
				PreliminaryGroupMessageHeader(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& time, MessageFlags const& messageFlags);
				PreliminaryGroupMessageHeader(PreliminaryGroupMessageHeader const& other);
				virtual ~PreliminaryGroupMessageHeader();

				virtual openmittsu::protocol::GroupId const& getGroup() const;

				virtual PreliminaryGroupMessageHeader* clone() const;
			private:
				openmittsu::protocol::GroupId const groupId;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_PRELIMINARYGROUPMESSAGEHEADER_H_
