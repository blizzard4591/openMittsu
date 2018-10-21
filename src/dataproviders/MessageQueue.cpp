#include "src/dataproviders/MessageQueue.h"

namespace openmittsu {
	namespace dataproviders {

		MessageQueue::ReceivedContactMessage::ReceivedContactMessage()
			: sender(0), messageId(0), timeSent(), timeReceived(), messageType(), content() {
		}

		MessageQueue::ReceivedGroupMessage::ReceivedGroupMessage() 
			: group(0, 0), sender(0), messageId(0), timeSent(), timeReceived(), messageType(), content(), contentTwo(), contentThree() {}

		MessageQueue::ReceivedGroupMessage::ReceivedGroupMessage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, messages::GroupMessageType const& messageType, QVariant const& content, QVariant const& contentTwo, QVariant const& contentThree)
			: group(group), sender(sender), messageId(messageId), timeSent(timeSent), timeReceived(timeReceived), messageType(messageType), content(content), contentTwo(contentTwo), contentThree(contentThree) {
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
			if (m_storedContactMessages.contains(message.sender)) {
				QVector<ReceivedContactMessage>& queue = *m_storedContactMessages.find(message.sender);
				queue.append(message);
			} else {
				QVector<ReceivedContactMessage> queue;
				queue.append(message);
				m_storedContactMessages.insert(message.sender, queue);
			}
		}

		void MessageQueue::storeGroupMessage(ReceivedGroupMessage const& message) {
			QMutexLocker lock(&m_mutex);
			if (m_storedGroupMessages.contains(message.group)) {
				QVector<ReceivedGroupMessage>& queue = *m_storedGroupMessages.find(message.group);
				queue.append(message);
			} else {
				QVector<ReceivedGroupMessage> queue;
				queue.append(message);
				m_storedGroupMessages.insert(message.group, queue);
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
