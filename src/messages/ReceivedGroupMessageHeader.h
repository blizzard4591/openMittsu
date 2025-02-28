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
			ReceivedGroupMessageHeader(FullMessageHeader const& other, openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageTime const& timeReceived);
			virtual ~ReceivedGroupMessageHeader();

			virtual openmittsu::protocol::GroupId const& getGroupId() const;
			
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<ReceivedGroupMessageHeader, true>;
#endif
		private:
			openmittsu::protocol::GroupId group;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::messages::ReceivedGroupMessageHeader)
Q_DECLARE_METATYPE(QSharedPointer<openmittsu::messages::ReceivedGroupMessageHeader const>)

#endif // OPENMITTSU_MESSAGES_RECEIVEDGROUPMESSAGEHEADER_H_
