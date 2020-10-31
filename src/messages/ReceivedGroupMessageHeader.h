#ifndef OPENMITTSU_MESSAGES_RECEIVEDGROUPMESSAGEHEADER_H_
#define OPENMITTSU_MESSAGES_RECEIVEDGROUPMESSAGEHEADER_H_

#include "src/messages/ReceivedMessageHeader.h"
#include "src/protocol/GroupId.h"

namespace openmittsu {
	namespace messages {
		class ReceivedGroupMessageHeader : public ReceivedMessageHeader {
		public:
			ReceivedGroupMessageHeader();
			ReceivedGroupMessageHeader(FullMessageHeader const& other, openmittsu::protocol::GroupId const& groupId);
			virtual ~ReceivedGroupMessageHeader();

			virtual openmittsu::protocol::GroupId const& getGroupId() const;
			
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ReceivedGroupMessageHeader, true>;
		private:
			openmittsu::protocol::GroupId group;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::messages::ReceivedGroupMessageHeader)
Q_DECLARE_METATYPE(QSharedPointer<openmittsu::messages::ReceivedGroupMessageHeader const>)

#endif // OPENMITTSU_MESSAGES_RECEIVEDGROUPMESSAGEHEADER_H_
