#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGEQUEUE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGEQUEUE_H_

#include <QHash>
#include <QMutex>
#include <QSet>
#include <QString>
#include <QVariant>

#include <cstdint>
#include <memory>

#include "src/utility/Location.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/dataproviders/messages/ContactMessageType.h"
#include "src/dataproviders/messages/GroupMessageType.h"

namespace openmittsu {
	namespace dataproviders {
		
		class MessageQueue {
		public:
			MessageQueue();
			virtual ~MessageQueue();

			class ReceivedContactMessage {
			public:
				ReceivedContactMessage();
				openmittsu::protocol::ContactId sender;
				openmittsu::protocol::MessageId messageId;
				openmittsu::protocol::MessageTime timeSent;
				openmittsu::protocol::MessageTime timeReceived;
				messages::ContactMessageType messageType;
				QVariant content;
			};

			class ReceivedGroupMessage {
			public:
				ReceivedGroupMessage();
				ReceivedGroupMessage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, messages::GroupMessageType const& messageType, QVariant const& content, QVariant const& contentTwo = QVariant(), QVariant const& contentThree = QVariant(), QVariant const& contentFour = QVariant(), QVariant const& contentFive = QVariant());
				
				openmittsu::protocol::GroupId group;
				openmittsu::protocol::ContactId sender;
				openmittsu::protocol::MessageId messageId;
				openmittsu::protocol::MessageTime timeSent;
				openmittsu::protocol::MessageTime timeReceived;
				messages::GroupMessageType messageType;
				QVariant content;
				QVariant contentTwo;
				QVariant contentThree;
				QVariant contentFour;
				QVariant contentFive;
			};

			void storeContactMessage(ReceivedContactMessage const& message);
			void storeGroupMessage(ReceivedGroupMessage const& message);

			bool hasMessageForGroup(openmittsu::protocol::GroupId const& group) const;

			QVector<ReceivedContactMessage> getAndRemoveQueuedMessages(openmittsu::protocol::ContactId const& sender);
			QVector<ReceivedGroupMessage> getAndRemoveQueuedMessages(openmittsu::protocol::GroupId const& group);
		private:
			QHash<openmittsu::protocol::ContactId, QVector<ReceivedContactMessage>> m_storedContactMessages;
			QHash<openmittsu::protocol::GroupId, QVector<ReceivedGroupMessage>> m_storedGroupMessages;
			QMutex m_mutex;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGEQUEUE_H_
