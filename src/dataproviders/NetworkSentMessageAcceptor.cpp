#include "src/dataproviders/NetworkSentMessageAcceptor.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/PreliminaryMessageFactory.h"
#include "src/network/ProtocolClient.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace dataproviders {

		NetworkSentMessageAcceptor::NetworkSentMessageAcceptor(std::weak_ptr<openmittsu::network::ProtocolClient> const& protocolClient) : SentMessageAcceptor(), m_protocolClient(protocolClient) {
			auto sPtr = m_protocolClient.lock();
			if (!sPtr) {
				throw openmittsu::exceptions::IllegalArgumentException() << "NetworkSentMessageAcceptor constructed with null ProtocolClient!";
			} else {
				OPENMITTSU_CONNECT_QUEUED(sPtr.get(), connectToFinished(int, QString), this, onConnectToFinished(int));
			}
		}

		NetworkSentMessageAcceptor::~NetworkSentMessageAcceptor() {
			//
		}

		void NetworkSentMessageAcceptor::send(openmittsu::messages::contact::PreliminaryContactMessage const& message) {
			auto sPtr = m_protocolClient.lock();
			if (sPtr) {
				if (!QMetaObject::invokeMethod(sPtr.get(), "sendContactMessage", Qt::QueuedConnection, Q_ARG(openmittsu::messages::contact::PreliminaryContactMessage, message))) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method sendContactMessage in " << __FILE__ << "  at line " << __LINE__ << ".";
				}
			} else {
				LOGGER()->error("NetworkSentMessageAcceptor::send(PreliminaryContactMessage) invoked, but the ProtocolClient pointer is null!");
			}
		}

		void NetworkSentMessageAcceptor::sendMessageReceivedAcknowledgement(openmittsu::protocol::ContactId const& messageSender, openmittsu::protocol::MessageId const& messageId) {
			auto sPtr = m_protocolClient.lock();
			if (sPtr) {
				if (!QMetaObject::invokeMethod(sPtr.get(), "sendMessageReceivedAcknowledgement", Qt::QueuedConnection, Q_ARG(openmittsu::protocol::ContactId, messageSender), Q_ARG(openmittsu::protocol::MessageId, messageId))) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method sendMessageReceivedAcknowledgement in " << __FILE__ << "  at line " << __LINE__ << ".";
				}
			} else {
				LOGGER()->error("NetworkSentMessageAcceptor::sendMessageReceivedAcknowledgement invoked, but the ProtocolClient pointer is null!");
			}
		}

		void NetworkSentMessageAcceptor::send(openmittsu::messages::group::PreliminaryGroupMessage const& message) {
			auto sPtr = m_protocolClient.lock();
			if (sPtr) {
				if (!QMetaObject::invokeMethod(sPtr.get(), "sendGroupMessage", Qt::QueuedConnection, Q_ARG(openmittsu::messages::group::PreliminaryGroupMessage, message))) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method sendGroupMessage in " << __FILE__ << "  at line " << __LINE__ << ".";
				}
			}
		}

		void NetworkSentMessageAcceptor::processSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactTextMessage(receiver, messageId, timeSent, message);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) {
			// Caption is already embedded, ignore.
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactImageMessage(receiver, messageId, timeSent, image);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactLocationMessage(receiver, messageId, timeSent, location);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(receiver, messageId, timeSent, referredMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(receiver, messageId, timeSent, referredMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(receiver, messageId, timeSent, referredMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(receiver, messageId, timeSent, referredMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupMessageText(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupTextMessage(group, messageId, timeSent, targetGroupMembers, message);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupMessageImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) {
			// Caption is already embedded, ignore.
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupImageMessage(group, messageId, timeSent, targetGroupMembers, image);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupLocationMessage(group, messageId, timeSent, targetGroupMembers, location);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QSet<openmittsu::protocol::ContactId> const& members) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupCreationMessage(group, messageId, timeSent, targetGroupMembers, members);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupSetImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupSetImageMessage(group, messageId, timeSent, targetGroupMembers, image);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupSetTitle(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& groupTitle) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupSetTitleMessage(group, messageId, timeSent, targetGroupMembers, groupTitle);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupSyncRequestMessage(group, messageId, timeSent);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentGroupLeave(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::ContactId const& ourContactId) {
			openmittsu::messages::group::PreliminaryGroupMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryGroupLeaveMessage(group, messageId, timeSent, targetGroupMembers, ourContactId);
			send(preliminaryMessage);
		}

		void NetworkSentMessageAcceptor::processSentContactMessageTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactUserTypingStartedMessage(receiver, messageId, timeSent);
			send(preliminaryMessage);
		}
		
		void NetworkSentMessageAcceptor::processSentContactMessageTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			openmittsu::messages::contact::PreliminaryContactMessage const preliminaryMessage = openmittsu::messages::PreliminaryMessageFactory::createPreliminaryContactUserTypingStoppedMessage(receiver, messageId, timeSent);
			send(preliminaryMessage);
		}

		bool NetworkSentMessageAcceptor::isConnected() const {
			auto sPtr = m_protocolClient.lock();
			if (sPtr == nullptr) {
				return false;
			}

			return sPtr->getIsConnected();
		}

		void NetworkSentMessageAcceptor::onConnectToFinished(int errCode) {
			if (errCode == 0) {
				emit readyToAcceptMessages();
			}
		}
	}
}
