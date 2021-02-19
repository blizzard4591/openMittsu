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
#include "src/messages/ReceivedMessageHeader.h"
#include "src/messages/ReceivedGroupMessageHeader.h"
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
				openmittsu::messages::ReceivedMessageHeader messageHeader;
				messages::ContactMessageType messageType;
				QVariant content;
			};

			class ReceivedGroupMessage {
			public:
				ReceivedGroupMessage();
				ReceivedGroupMessage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, messages::GroupMessageType const& messageType, QVariant const& content, QVariant const& contentTwo = QVariant(), QVariant const& contentThree = QVariant(), QVariant const& contentFour = QVariant(), QVariant const& contentFive = QVariant());
				
				openmittsu::messages::ReceivedGroupMessageHeader messageHeader;
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

			bool hasGroupIdForPartial(quint64 const groupId, std::unique_ptr<openmittsu::protocol::GroupId>& target) const;
		private:
			QHash<openmittsu::protocol::ContactId, QVector<ReceivedContactMessage>> m_storedContactMessages;
			QHash<openmittsu::protocol::GroupId, QVector<ReceivedGroupMessage>> m_storedGroupMessages;
			mutable QMutex m_mutex;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGEQUEUE_H_
