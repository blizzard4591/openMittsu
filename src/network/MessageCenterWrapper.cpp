#include "src/network/MessageCenterWrapper.h"

#include <QMetaObject>
#include "src/dataproviders/MessageCenter.h"
#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace network {

		MessageCenterWrapper::MessageCenterWrapper(std::shared_ptr<openmittsu::dataproviders::MessageCenter> const& messageCenter) : openmittsu::dataproviders::ReceivedMessageAcceptor(), m_messageCenter(messageCenter) {
			//
		}

		MessageCenterWrapper::~MessageCenterWrapper() {
			//
		}

		bool MessageCenterWrapper::hasMessageCenter() const {
			return (m_messageCenter) ? true : false;
		}

		void MessageCenterWrapper::addNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "onFoundNewContact", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, contact), Q_ARG(openmittsu::crypto::PublicKey, publicKey))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method onFoundNewContact in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, receiver), Q_ARG(openmittsu::protocol::MessageId, messageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method onMessageSendFailed in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}
		
		void MessageCenterWrapper::processMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendDone", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, receiver), Q_ARG(openmittsu::protocol::MessageId, messageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method onMessageSendDone in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}
		
		void MessageCenterWrapper::processMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::MessageId, messageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method onMessageSendFailed in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}
		
		void MessageCenterWrapper::processMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "onMessageSendDone", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::MessageId, messageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method onMessageSendDone in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageText", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, message))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageText in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageImage in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageLocation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(openmittsu::utility::Location, location))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageLocation in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptReceived", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageReceiptReceived in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptSeen", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageReceiptSeen in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptAgree", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageReceiptAgree in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactMessageReceiptDisagree", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageId, referredMessageId))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactMessageReceiptDisagree in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactTypingNotificationTyping", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactTypingNotificationTyping in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}
		
		void MessageCenterWrapper::processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedContactTypingNotificationStopped", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedContactTypingNotificationStopped in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageText", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, message))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupMessageText in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupMessageImage in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupMessageLocation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(openmittsu::utility::Location, location))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupMessageLocation in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupCreation", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QSet<openmittsu::protocol::ContactId>, members))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupCreation in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSetImage", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QByteArray, image))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupSetImage in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSetTitle", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived), Q_ARG(QString, groupTitle))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupSetTitle in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupSyncRequest", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupSyncRequest in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

		void MessageCenterWrapper::processReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (!QMetaObject::invokeMethod(m_messageCenter.get(), "processReceivedGroupLeave", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::GroupId, group), Q_ARG(openmittsu::protocol::ContactId, sender), Q_ARG(openmittsu::protocol::MessageId, messageId), Q_ARG(openmittsu::protocol::MessageTime, timeSent), Q_ARG(openmittsu::protocol::MessageTime, timeReceived))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method processReceivedGroupLeave in " << __FILE__ << "  at line " << __LINE__ << ".";
			}
		}

	}
}