#include "src/dataproviders/MessageQueue.h"

namespace openmittsu {
	namespace dataproviders {

		MessageQueue::ReceivedContactMessage::ReceivedContactMessage()
			: messageHeader(), messageType(), content() {
		}

		MessageQueue::ReceivedGroupMessage::ReceivedGroupMessage() 
			: messageHeader(), messageType(), content(), contentTwo(), contentThree() {}

		MessageQueue::ReceivedGroupMessage::ReceivedGroupMessage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, messages::GroupMessageType const& messageType, QVariant const& content, QVariant const& contentTwo, QVariant const& contentThree, QVariant const& contentFour, QVariant const& contentFive)
			: messageHeader(messageHeader), messageType(messageType), content(content), contentTwo(contentTwo), contentThree(contentThree), contentFour(contentFour), contentFive(contentFive) {
			//
		}

		MessageQueue::MessageQueue() {
			//
		}
		
		MessageQueue::~MessageQueue() {
			//
		}

		void MessageQueue::storeContactMessage(ReceivedContactMessage const& message) {
			QMutexLocker lock(&m_mutex);
			if (m_storedContactMessages.contains(message.messageHeader.getSender())) {
				QVector<ReceivedContactMessage>& queue = *m_storedContactMessages.find(message.messageHeader.getSender());
				queue.append(message);
			} else {
				QVector<ReceivedContactMessage> queue;
				queue.append(message);
				m_storedContactMessages.insert(message.messageHeader.getSender(), queue);
			}
		}

		void MessageQueue::storeGroupMessage(ReceivedGroupMessage const& message) {
			QMutexLocker lock(&m_mutex);
			if (m_storedGroupMessages.contains(message.messageHeader.getGroupId())) {
				QVector<ReceivedGroupMessage>& queue = *m_storedGroupMessages.find(message.messageHeader.getGroupId());
				queue.append(message);
			} else {
				QVector<ReceivedGroupMessage> queue;
				queue.append(message);
				m_storedGroupMessages.insert(message.messageHeader.getGroupId(), queue);
			}
		}

		bool MessageQueue::hasMessageForGroup(openmittsu::protocol::GroupId const& group) const {
			if (m_storedGroupMessages.contains(group)) {
				return m_storedGroupMessages.constFind(group)->size() > 0;
			}
			return false;
		}

		QVector<MessageQueue::ReceivedContactMessage> MessageQueue::getAndRemoveQueuedMessages(openmittsu::protocol::ContactId const& sender) {
			QMutexLocker lock(&m_mutex);
			if (m_storedContactMessages.contains(sender)) {
				QVector<ReceivedContactMessage> queue = *m_storedContactMessages.find(sender);
				m_storedContactMessages.remove(sender);
				return queue;
			} else {
				return QVector<ReceivedContactMessage>();
			}
		}

		QVector<MessageQueue::ReceivedGroupMessage> MessageQueue::getAndRemoveQueuedMessages(openmittsu::protocol::GroupId const& group) {
			QMutexLocker lock(&m_mutex);
			if (m_storedGroupMessages.contains(group)) {
				QVector<ReceivedGroupMessage> queue = *m_storedGroupMessages.find(group);
				m_storedGroupMessages.remove(group);
				return queue;
			} else {
				return QVector<ReceivedGroupMessage>();
			}
		}

	}
}
