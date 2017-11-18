#include "src/dataproviders/MessageCenter.h"

#include "src/database/Database.h"
#include "src/dataproviders/NetworkSentMessageAcceptor.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/messages/PreliminaryMessageFactory.h"
#include "src/utility/Logging.h"
#include "src/utility/QExifImageHeader.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/widgets/chat/ChatTabWidget.h"
#include "src/widgets/SimpleContactChatTab.h"
#include "src/widgets/SimpleGroupChatTab.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>

namespace openmittsu {
	namespace dataproviders {
		MessageCenter::MessageCenter(std::shared_ptr<openmittsu::widgets::TabController> const& tabController, std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster) : QObject(), m_tabController(tabController), m_optionMaster(optionMaster), m_networkSentMessageAcceptor(nullptr), m_storage(nullptr) {
			if ((tabController == nullptr) || (optionMaster == nullptr)) {
				throw openmittsu::exceptions::IllegalArgumentException() << "MessageCenter created with a TabController or OptionMaster that is null!";
			}
		}

		MessageCenter::~MessageCenter() {
			//
		}

		void MessageCenter::databaseOnMessageChanged(QString const& uuid) {
			emit messageChanged(uuid);
		}

		void MessageCenter::databaseOnContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const&) {
			if (m_tabController) {
				if (m_tabController->hasTab(identity)) {
					openmittsu::widgets::ChatTab* const chatTab = m_tabController->getTab(identity);
					emit newUnreadMessageAvailable(chatTab);
				}
			}
		}
		
		void MessageCenter::databaseOnGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const&) {
			if (m_tabController) {
				if (m_tabController->hasTab(group)) {
					openmittsu::widgets::ChatTab* const chatTab = m_tabController->getTab(group);
					emit newUnreadMessageAvailable(chatTab);
				}
			}
		}

		bool MessageCenter::sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasContact(receiver)) {
				LOGGER()->warn("Trying to send text message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentContactMessageText(receiver, sentTime, willQueue, text);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageText(receiver, messageId, sentTime, text);
			}

			return true;
		}

		bool MessageCenter::sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasContact(receiver)) {
				LOGGER()->warn("Trying to send image message to unknown contact {}", receiver.toString());
				return false;
			}

			QByteArray imageBytes = image;
			embedCaptionIntoImage(imageBytes, caption);

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentContactMessageImage(receiver, sentTime, willQueue, imageBytes, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageImage(receiver, messageId, sentTime, imageBytes, caption);
			}

			return true;
		}

		bool MessageCenter::sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasContact(receiver)) {
				LOGGER()->warn("Trying to send location message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentContactMessageLocation(receiver, sentTime, willQueue, location);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageLocation(receiver, messageId, sentTime, location);
			}

			return true;
		}

		void MessageCenter::sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) {
			if (m_optionMaster == nullptr) {
				LOGGER()->warn("MessageCenter has a null OptionMaster!");
				return;
			}

			if (!m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_SEND_TYPING_NOTIFICATION)) {
				return;
			}

			if (this->m_storage == nullptr) {
				return;
			} else if (!this->m_storage->hasContact(receiver)) {
				LOGGER()->warn("Trying to send typing message to unknown contact {}", receiver.toString());
				return;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}
			
			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId messageId(0);
			if (isTyping) {
				messageId = this->m_storage->storeSentContactMessageNotificationTypingStarted(receiver, sentTime, willQueue);
			} else {
				messageId = this->m_storage->storeSentContactMessageNotificationTypingStopped(receiver, sentTime, willQueue);
			}

			if (willQueue) {
				if (isTyping) {
					m_networkSentMessageAcceptor->processSentContactMessageTypingStarted(receiver, messageId, sentTime);
				} else {
					m_networkSentMessageAcceptor->processSentContactMessageTypingStopped(receiver, messageId, sentTime);
				}
			}
		}

		bool MessageCenter::sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasContact(receiver)) {
				LOGGER()->warn("Trying to send receipt message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}
			
			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId messageId(0);
			switch (receiptType) {
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED:
					messageId = m_storage->storeSentContactMessageReceiptReceived(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
					messageId = m_storage->storeSentContactMessageReceiptSeen(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE:
					messageId = m_storage->storeSentContactMessageReceiptAgree(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE:
					messageId = m_storage->storeSentContactMessageReceiptDisagree(receiver, sentTime, willQueue, receiptedMessageId);
					break;
			}


			if (willQueue) {
				switch (receiptType) {
					case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED:
						m_networkSentMessageAcceptor->processSentContactMessageReceiptReceived(receiver, messageId, sentTime, receiptedMessageId);
						break;
					case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
						m_networkSentMessageAcceptor->processSentContactMessageReceiptSeen(receiver, messageId, sentTime, receiptedMessageId);
						break;
					case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE:
						m_networkSentMessageAcceptor->processSentContactMessageReceiptAgree(receiver, messageId, sentTime, receiptedMessageId);
						break;
					case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE:
						m_networkSentMessageAcceptor->processSentContactMessageReceiptDisagree(receiver, messageId, sentTime, receiptedMessageId);
						break;
				}
			}

			return true;
		}

		bool MessageCenter::sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasGroup(group)) {
				LOGGER()->warn("Trying to send receipt message to unknown group {}", group.toString());
				return false;
			}

			switch (receiptType) {
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
					//this->m_storage->store(group, receiptedopenmittsu::protocol::MessageId, openmittsu::protocol::MessageTime::now());
					break;
				default:
					LOGGER()->warn("Trying to send a receipt \"{}\" for message ID {} to group {}, this should never happen.", static_cast<int>(receiptType), receiptedMessageId.toString(), group.toString());
					return false;
			}

			return true;
		}

		bool MessageCenter::sendLeave(openmittsu::protocol::GroupId const& group) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() == group.getOwner()) {
				LOGGER()->warn("Trying to send leave message to group {} which is owned by us.", group.toString());
				return false;
			}
			
			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupLeave(group, sentTime, willQueue, true);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupLeave(group, this->m_storage->getGroupMembers(group, true), messageId, sentTime, this->m_storage->getSelfContact());
			}

			return true;
		}

		bool MessageCenter::sendSyncRequest(openmittsu::protocol::GroupId const& group) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() == group.getOwner()) {
				LOGGER()->warn("Trying to send sync request message to group {} which is owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupSyncRequest(group, sentTime, willQueue);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSyncRequest(group, this->m_storage->getGroupMembers(group, true), messageId, sentTime);
			}

			return true;
		}

		bool MessageCenter::sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group creation message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupCreation(group, members, m_storage->getGroupMembers(group, true), true);
		}

		bool MessageCenter::sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group creation message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupCreation(group, sentTime, willQueue, members, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupCreation(group, recipients, messageId, sentTime, members);
			}

			return true;
		}
		
		bool MessageCenter::sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group title message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupTitle(group, title, m_storage->getGroupMembers(group, true), true);
		}

		bool MessageCenter::sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group title message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupSetTitle(group, sentTime, willQueue, title, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSetTitle(group, recipients, messageId, sentTime, title);
			}

			return true;
		}
		
		bool MessageCenter::sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group image message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupImage(group, image, m_storage->getGroupMembers(group, true), true);
		}

		bool MessageCenter::sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (this->m_storage->getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group image message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupSetImage(group, sentTime, willQueue, image, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSetImage(group, recipients, messageId, sentTime, image);
			}

			return true;
		}

		bool MessageCenter::sendText(openmittsu::protocol::GroupId const& group, QString const& text) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasGroup(group)) {
				LOGGER()->warn("Trying to send text message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupMessageText(group, sentTime, willQueue, text);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageText(group, this->m_storage->getGroupMembers(group, true), messageId, sentTime, text);
			}

			return true;
		}

		bool MessageCenter::sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasGroup(group)) {
				LOGGER()->warn("Trying to send image message to unknown group {}", group.toString());
				return false;
			}

			QByteArray imageBytes = image;
			embedCaptionIntoImage(imageBytes, caption);

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupMessageImage(group, sentTime, willQueue, imageBytes, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageImage(group, this->m_storage->getGroupMembers(group, true), messageId, sentTime, imageBytes, caption);
			}

			return true;
		}

		bool MessageCenter::sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) {
			if (this->m_storage == nullptr) {
				return false;
			} else if (!this->m_storage->hasGroup(group)) {
				LOGGER()->warn("Trying to send location message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage->storeSentGroupMessageLocation(group, sentTime, willQueue, location);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageLocation(group, this->m_storage->getGroupMembers(group, true), messageId, sentTime, location);
			}

			return true;
		}

		void MessageCenter::setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) {
			this->m_networkSentMessageAcceptor = newNetworkSentMessageAcceptor;
			if (m_networkSentMessageAcceptor) {
				OPENMITTSU_CONNECT(m_networkSentMessageAcceptor.get(), readyToAcceptMessages(), this, tryResendingMessagesToNetwork());
				tryResendingMessagesToNetwork();
			}
		}

		void MessageCenter::tryResendingMessagesToNetwork() {
			if ((this->m_networkSentMessageAcceptor != nullptr) && (this->m_networkSentMessageAcceptor->isConnected()) && (this->m_storage != nullptr)) {
				LOGGER()->info("Asking database to send all queued messges now...");
				this->m_storage->sendAllWaitingMessages(*m_networkSentMessageAcceptor);
			}
		}

		void MessageCenter::setStorage(std::shared_ptr<openmittsu::dataproviders::MessageStorage> const& newStorage) {
			if (m_storage != nullptr) {
				OPENMITTSU_DISCONNECT(dynamic_cast<QObject*>(m_storage.get()), messageChanged(QString const&), this, databaseOnMessageChanged(QString const&));
				OPENMITTSU_DISCONNECT(dynamic_cast<QObject*>(m_storage.get()), haveQueuedMessages(), this, tryResendingMessagesToNetwork());
				OPENMITTSU_DISCONNECT(dynamic_cast<QObject*>(newStorage.get()), contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, databaseOnContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));
				OPENMITTSU_DISCONNECT(dynamic_cast<QObject*>(newStorage.get()), groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, databaseOnGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
			}

			this->m_storage = newStorage;
			OPENMITTSU_CONNECT(dynamic_cast<QObject*>(newStorage.get()), messageChanged(QString const&), this, databaseOnMessageChanged(QString const&));
			OPENMITTSU_CONNECT(dynamic_cast<QObject*>(newStorage.get()), haveQueuedMessages(), this, tryResendingMessagesToNetwork());
			OPENMITTSU_CONNECT(dynamic_cast<QObject*>(newStorage.get()), contactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&), this, databaseOnContactHasNewMessage(openmittsu::protocol::ContactId const&, QString const&));
			OPENMITTSU_CONNECT(dynamic_cast<QObject*>(newStorage.get()), groupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&), this, databaseOnGroupHasNewMessage(openmittsu::protocol::GroupId const&, QString const&));
		}

		/*
			Received Messages from Network
		*/
		void MessageCenter::processReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact text message from sender {} with message ID #{} sent at {} with text {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), message.toStdString());
				return;
			} else if (!this->m_storage->hasContact(sender)) {

			}

			openTabForIncomingMessage(sender);
			this->m_storage->storeReceivedContactMessageText(sender, messageId, timeSent, timeReceived, message);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}

			sendReceipt(sender, messageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void MessageCenter::processReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact image message from sender {} with message ID #{} sent at {} with image {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), QString(image.toHex()).toStdString());
				return;
			}

			QString const caption = parseCaptionFromImage(image);

			openTabForIncomingMessage(sender);
			this->m_storage->storeReceivedContactMessageImage(sender, messageId, timeSent, timeReceived, image, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}

			sendReceipt(sender, messageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}
		
		void MessageCenter::processReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact location message from sender {} with message ID #{} sent at {} with location {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), location.toString());
				return;
			}

			openTabForIncomingMessage(sender);
			this->m_storage->storeReceivedContactMessageLocation(sender, messageId, timeSent, timeReceived, location);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}

			sendReceipt(sender, messageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void MessageCenter::processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact message receipt type RECEIVED from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type RECEIVED from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			this->m_storage->storeReceivedContactMessageReceiptReceived(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact message receipt type SEEN from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type SEEN from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			this->m_storage->storeReceivedContactMessageReceiptSeen(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact message receipt type AGREE from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type AGREE from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			openTabForIncomingMessage(sender);
			this->m_storage->storeReceivedContactMessageReceiptAgree(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a contact message receipt type DISAGREE from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type DISAGREE from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			openTabForIncomingMessage(sender);
			this->m_storage->storeReceivedContactMessageReceiptDisagree(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a typing start notification from sender {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			LOGGER_DEBUG("We received a typing start notification from sender {} with message ID #{} sent at {}.", sender.toString(), messageId.toString(), timeSent.toString());
			this->m_storage->storeReceivedContactTypingNotificationTyping(sender, messageId, timeSent);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a typing stop notification from sender {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			LOGGER_DEBUG("We received a typing stop notification from sender {} with message ID #{} sent at {}.", sender.toString(), messageId.toString(), timeSent.toString());
			this->m_storage->storeReceivedContactTypingNotificationStopped(sender, messageId, timeSent);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group text message from sender {} for group {} with message ID #{} sent at {} with text {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), message.toStdString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::TEXT, message));
				return;
			}

			openTabForIncomingMessage(group);
			this->m_storage->storeReceivedGroupMessageText(group, sender, messageId, timeSent, timeReceived, message);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group image message from sender {} for group {} with message ID #{} sent at {} with image {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), QString(image.toHex()).toStdString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::IMAGE, image));
				return;
			}

			QString const caption = parseCaptionFromImage(image);

			openTabForIncomingMessage(group);
			this->m_storage->storeReceivedGroupMessageImage(group, sender, messageId, timeSent, timeReceived, image, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group location message from sender {} for group {} with message ID #{} sent at {} with location {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), location.toString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				QVariant locationVariant;
				locationVariant.setValue(location);
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::LOCATION, locationVariant));
				return;
			}

			openTabForIncomingMessage(group);
			this->m_storage->storeReceivedGroupMessageLocation(group, sender, messageId, timeSent, timeReceived, location);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}


		void MessageCenter::processReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
			if (this->m_storage == nullptr) {
				QString memberString;
				for (openmittsu::protocol::ContactId const&c : members) {
					if (!memberString.isEmpty()) {
						memberString.append(QStringLiteral(", "));
					}
					memberString.append(c.toQString());
				}

				LOGGER()->warn("We received a group creation message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), memberString.toStdString());
				return;
			} else if (sender != group.getOwner()) {
				QString memberString;
				for (openmittsu::protocol::ContactId const&c : members) {
					if (!memberString.isEmpty()) {
						memberString.append(QStringLiteral(", "));
					}
					memberString.append(c.toQString());
				}

				LOGGER()->warn("We received a group creation message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), memberString.toStdString());
				return;
			}

			this->m_storage->storeReceivedGroupCreation(group, sender, messageId, timeSent, timeReceived, members);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}

			QVector<MessageQueue::ReceivedGroupMessage> queuedMessages = m_messageQueue.getAndRemoveQueuedMessages(group);
			auto it = queuedMessages.constBegin();
			auto const end = queuedMessages.constEnd();
			for (; it != end; ++it) {
				messages::GroupMessageType const messageType = it->messageType;
				switch (messageType) {
					case messages::GroupMessageType::IMAGE:
						processReceivedGroupMessageImage(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived, it->content.toByteArray());
						break;
					case messages::GroupMessageType::LEAVE:
						processReceivedGroupLeave(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived);
						break;
					case messages::GroupMessageType::LOCATION:
						processReceivedGroupMessageLocation(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived, it->content.value<openmittsu::utility::Location>());
						break;
					case messages::GroupMessageType::SET_IMAGE:
						processReceivedGroupSetImage(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived, it->content.toByteArray());
						break;
					case messages::GroupMessageType::SET_TITLE:
						processReceivedGroupSetTitle(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived, it->content.toString());
						break;
					case messages::GroupMessageType::SYNC_REQUEST:
						processReceivedGroupSyncRequest(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived);
						break;
					case messages::GroupMessageType::TEXT:
						processReceivedGroupMessageText(it->group, it->sender, it->messageId, it->timeSent, it->timeReceived, it->content.toString());
						break;
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Group Message queue contains a message of type \"" << messages::GroupMessageTypeHelper::toString(messageType) << "\", which is unhandled. This should never happen!";
				}
			}
		}

		void MessageCenter::processReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group set image message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), QString(image.toHex()).toStdString());
				return;
			} else if (sender != group.getOwner()) {
				LOGGER()->warn("We received a group set image message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), QString(image.toHex()).toStdString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::SET_IMAGE, image));
				return;
			}

			this->m_storage->storeReceivedGroupSetImage(group, sender, messageId, timeSent, timeReceived, image);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), groupTitle.toStdString());
				return;
			} else if (sender != group.getOwner()) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString(), groupTitle.toStdString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::SET_TITLE, groupTitle));
				return;
			}

			this->m_storage->storeReceivedGroupSetTitle(group, sender, messageId, timeSent, timeReceived, groupTitle);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::processReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString());
				return;
			} else if (sender == group.getOwner()) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {} that did come from the group owner. Ignoring.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString());
				return;
			} else if (group.getOwner() != m_storage->getSelfContact()) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {}, but we are not the group owner. Ignoring.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::SYNC_REQUEST, QVariant()));
				return;
			}

			this->m_storage->storeReceivedGroupSyncRequest(group, sender, messageId, timeSent, timeReceived);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}

			this->resendGroupSetup(group, {sender});
		}

		void MessageCenter::processReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We received a group leave message from sender {} for group {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), group.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			if (!checkAndFixGroupMembership(group, sender)) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(group, sender, messageId, timeSent, timeReceived, messages::GroupMessageType::LEAVE, QVariant()));
				return;
			}

			this->m_storage->storeReceivedGroupLeave(group, sender, messageId, timeSent, timeReceived);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void MessageCenter::onMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} failed, but that could not be saved as the storage system is not ready.", receiver.toString(), messageId.toString());
				return;
			}
			this->m_storage->storeMessageSendFailed(receiver, messageId);
		}

		void MessageCenter::onMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} was successful, but that could not be saved as the storage system is not ready.", receiver.toString(), messageId.toString());
				return;
			}
			this->m_storage->storeMessageSendDone(receiver, messageId);
		}

		void MessageCenter::onMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were notfied that sending a message to group {} with message ID #{} failed, but that could not be saved as the storage system is not ready.", group.toString(), messageId.toString());
				return;
			}
			this->m_storage->storeMessageSendFailed(group, messageId);
		}

		void MessageCenter::onMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were notfied that sending a message to group {} with message ID #{} was successful, but that could not be saved as the storage system is not ready.", group.toString(), messageId.toString());
				return;
			}
			this->m_storage->storeMessageSendDone(group, messageId);
		}

		void MessageCenter::onFoundNewContact(openmittsu::protocol::ContactId const& newContact, openmittsu::crypto::PublicKey const& publicKey) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were notfied of a new contact with ID {}, but that could not be saved as the storage system is not ready.", newContact.toString());
				return;
			}

			this->m_storage->storeNewContact(newContact, publicKey);
		}

		void MessageCenter::onFoundNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members) {
			if (this->m_storage == nullptr) {
				QString memberString;
				for (openmittsu::protocol::ContactId const&c : members) {
					if (!memberString.isEmpty()) {
						memberString.append(QStringLiteral(", "));
					}
					memberString.append(c.toQString());
				}

				LOGGER()->warn("We were notfied of a new group with ID {} and members {}, but that could not be saved as the storage system is not ready.", groupId.toString(), memberString.toStdString());
				return;
			}

			this->m_storage->storeNewGroup(groupId, members, false);
		}

		bool MessageCenter::createNewGroupAndInformMembers(QSet<openmittsu::protocol::ContactId> const& members, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) {
			if (this->m_storage == nullptr) {
				return false;
			}

			openmittsu::protocol::ContactId const selfContactId = m_storage->getSelfContact();
			openmittsu::protocol::GroupId groupId = openmittsu::protocol::GroupId::createRandomGroupId(selfContactId);
			while (m_storage->hasGroup(groupId)) {
				groupId = openmittsu::protocol::GroupId::createRandomGroupId(selfContactId);
			}

			QSet<openmittsu::protocol::ContactId> groupMembers = members;
			if (addSelfContact) {
				groupMembers.insert(selfContactId);
			}

			this->m_storage->storeNewGroup(groupId, groupMembers, false);

			sendGroupCreation(groupId, groupMembers);
			if (!groupTitle.isNull() && groupTitle.canConvert<QString>()) {
				sendGroupTitle(groupId, groupTitle.toString());
			}
			if (!groupImage.isNull() && groupImage.canConvert<QByteArray>()) {
				sendGroupImage(groupId, groupImage.toByteArray());
			}

			return true;
		}

		void MessageCenter::resendGroupSetup(openmittsu::protocol::GroupId const& group) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were asked to re-send the group setup for group {}, but that could not be done as the storage system is not ready.", group.toString());
				return;
			}

			resendGroupSetup(group, m_storage->getGroupMembers(group, true));
		}

		void MessageCenter::resendGroupSetup(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& recipients) {
			if (this->m_storage == nullptr) {
				LOGGER()->warn("We were asked to re-send the group setup for group {}, but that could not be done as the storage system is not ready.", group.toString());
				return;
			}

			openmittsu::dataproviders::BackedGroup backedGroup = m_storage->getBackedGroup(group, *this);

			sendGroupCreation(group, m_storage->getGroupMembers(group, false), recipients, false);
			sendGroupTitle(group, backedGroup.getTitle(), recipients, false);
			if (backedGroup.hasImage()) {
				sendGroupImage(group, backedGroup.getImage(), recipients, false);
			}
		}

		QString MessageCenter::parseCaptionFromImage(QByteArray const& image) const {
			QExifImageHeader header;
			QBuffer buffer;
			buffer.setData(image);
			buffer.open(QBuffer::ReadOnly);

			QString imageText = QStringLiteral("");
			if (header.loadFromJpeg(&buffer)) {
				if (header.contains(QExifImageHeader::Artist)) {
					LOGGER_DEBUG("Image has Artist Tag: {}", header.value(QExifImageHeader::Artist).toString().toStdString());
					imageText = header.value(QExifImageHeader::Artist).toString();
				} else if (header.contains(QExifImageHeader::UserComment)) {
					LOGGER_DEBUG("Image has UserComment Tag: {}", header.value(QExifImageHeader::UserComment).toString().toStdString());
					imageText = header.value(QExifImageHeader::UserComment).toString();
				} else {
					LOGGER_DEBUG("Image does not have Artist or UserComment Tag.");
				}
			} else {
				LOGGER_DEBUG("Image does not have an EXIF Tag.");
			}

			return imageText;
		}

		void MessageCenter::embedCaptionIntoImage(QByteArray& image, QString const& caption) const {
			if (!caption.isEmpty()) {
				QBuffer buffer(&image);
				buffer.open(QIODevice::ReadWrite);

				QExifImageHeader header;
				header.setValue(QExifImageHeader::UserComment, QExifValue(caption));
				buffer.seek(0);
				header.saveToJpeg(&buffer);
				buffer.close();
			}
		}

		void MessageCenter::openTabForIncomingMessage(openmittsu::protocol::ContactId const& sender) {
			if (this->m_tabController != nullptr) {
				if (!this->m_tabController->hasTab(sender)) {
					this->m_tabController->openTab(sender, this->m_storage->getBackedContact(sender, *this));
				}
			}
		}
		
		void MessageCenter::openTabForIncomingMessage(openmittsu::protocol::GroupId const& group) {
			if (this->m_tabController != nullptr) {
				if (!this->m_tabController->hasTab(group)) {
					this->m_tabController->openTab(group, this->m_storage->getBackedGroup(group, *this));
				}
			}
		}

		void MessageCenter::requestSyncForGroupIfApplicable(openmittsu::protocol::GroupId const& group) {
			this->sendSyncRequest(group);
		}

		bool MessageCenter::checkAndFixGroupMembership(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender) {
			if (this->m_storage == nullptr) {
				return false;
			} else {
				//	if group is unknown
				//		if owned by us
				//			if option TRUST_OTHERS
				//				(re-)create (from deleted if so) with sender + us added, save message
				//			else
				//				ignore
				//		else
				//			request sync if not done so in last x time
				//			if option TRUST_OTHERS
				//				(re-)create (from deleted if so) with sender + us added, save message
				//			else
				//				ignore
				//	else // group is known
				//		if sender is in group
				//			accept, save message
				//		else
				//			if owned by us
				//				ignore
				//			else
				//				request sync if not done so in last x time
				//				if option TRUST_OTHERS
				//					add sender, save message
				//				else
				//					ignore

				if (!this->m_storage->hasGroup(group)) {
					if (group.getOwner() != this->m_storage->getSelfContact()) {
						requestSyncForGroupIfApplicable(group);
					}

					if (this->m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_TRUST_OTHERS)) {
						QSet<openmittsu::protocol::ContactId> groupMembers;
						if (this->m_storage->isDeleteted(group)) {
							groupMembers = this->m_storage->getGroupMembers(group, false); // need not exclude, as the group is deleted we are not in there anyway
						}
						groupMembers.insert(this->m_storage->getSelfContact());
						groupMembers.insert(sender);
						this->m_storage->storeNewGroup(group, groupMembers, true);

						return true;
					} else {
						return false;
					}
				} else {
					if (this->m_storage->getGroupMembers(group, false).contains(sender)) {
						return true;
					} else {
						if (group.getOwner() == this->m_storage->getSelfContact()) {
							return false;
						} else {
							requestSyncForGroupIfApplicable(group);
							if (this->m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_TRUST_OTHERS)) {
								QSet<openmittsu::protocol::ContactId> groupMembers = this->m_storage->getGroupMembers(group, false);
								groupMembers.insert(this->m_storage->getSelfContact());
								groupMembers.insert(sender);
								this->m_storage->storeNewGroup(group, groupMembers, true);

								return true;
							} else {
								return false;
							}
						}
					}
				}
			}
		}

	}
}

