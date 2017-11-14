#include "src/network/MessageCenterWrapper.h"

#include <QMetaObject>
#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace network {

		MessageCenterWrapper::MessageCenterWrapper(std::shared_ptr<openmittsu::dataproviders::MessageCenter> const& messageCenter) : openmittsu::dataproviders::ReceivedMessageAcceptor(), m_messageCenter(messageCenter) {
			//
		}

		MessageCenterWrapper::~MessageCenterWrapper() {
			//
		}

		void MessageCenterWrapper::addNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "onFoundNewContact", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, contact), Q_ARG(openmittsu::crypto::PublicKey, publicKey));
		}

		void MessageCenterWrapper::processMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, receiver), Q_ARG(openmittsu::protocol::MessageId, messageId));
		}
		
		void MessageCenterWrapper::processMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendDone", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, receiver), Q_ARG(openmittsu::protocol::MessageId, messageId));
		}
		
		void MessageCenterWrapper::processMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::MessageId, messageId));
		}
		
		void MessageCenterWrapper::processMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendDone", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::MessageId, messageId));
		}

		void MessageCenterWrapper::processReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageText", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, message));
		}

		void MessageCenterWrapper::processReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image));
		}

		void MessageCenterWrapper::processReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageLocation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(openmittsu::utility::Location, location));
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptReceived", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId));
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptSeen", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId));
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptAgree", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId));
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptDisagree", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId));
		}

		void MessageCenterWrapper::processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactTypingNotificationTyping", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent));
		}
		
		void MessageCenterWrapper::processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactTypingNotificationStopped", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent));
		}

		void MessageCenterWrapper::processReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageText", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, message));
		}

		void MessageCenterWrapper::processReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image));
		}

		void MessageCenterWrapper::processReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageLocation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(openmittsu::utility::Location, location));
		}

		void MessageCenterWrapper::processReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupCreation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QSet<openmittsu::protocol::ContactId>, members));
		}

		void MessageCenterWrapper::processReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSetImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image));
		}

		void MessageCenterWrapper::processReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSetTitle", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, groupTitle));
		}

		void MessageCenterWrapper::processReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSetTitle", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived));
		}

		void MessageCenterWrapper::processReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupLeave", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived));
		}

	}
}