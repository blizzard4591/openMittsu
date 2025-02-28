#include "src/dataproviders/SimpleMessageCenter.h"

#include "src/dataproviders/MessageQueue.h"
#include "src/dataproviders/NetworkSentMessageAcceptor.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/messages/PreliminaryMessageFactory.h"
#include "src/protocol/ContactIdList.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QExifImageHeader.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QBuffer>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <memory>

namespace openmittsu {
	namespace dataproviders {

		SimpleMessageCenter::SimpleMessageCenter(openmittsu::database::DatabaseWrapperFactory const& databaseWrapperFactory) : MessageCenter(), m_optionReader(databaseWrapperFactory.getDatabaseWrapper()), m_networkSentMessageAcceptor(nullptr), m_storage(databaseWrapperFactory.getDatabaseWrapper()) {
			OPENMITTSU_CONNECT(&m_storage, messageChanged(QString const&), this, databaseOnMessageChanged(QString const&));
			OPENMITTSU_CONNECT(&m_storage, messageDeleted(QString const&), this, databaseOnMessageDeleted(QString const&));
			OPENMITTSU_CONNECT(&m_storage, haveQueuedMessages(), this, tryResendingMessagesToNetwork());
		}

		SimpleMessageCenter::~SimpleMessageCenter() {
			//
		}

		void SimpleMessageCenter::databaseOnMessageChanged(QString const& uuid) {
			emit messageChanged(uuid);
		}

		void SimpleMessageCenter::databaseOnMessageDeleted(QString const& uuid) {
			emit messageDeleted(uuid);
		}

		bool SimpleMessageCenter::sendAudio(openmittsu::protocol::ContactId const& receiver, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("Trying to send image message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageAudio(receiver, sentTime, willQueue, audio, lengthInSeconds);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageAudio(receiver, messageId, sentTime, audio, lengthInSeconds);
			}

			return true;
		}

		bool SimpleMessageCenter::sendFile(openmittsu::protocol::ContactId const& receiver, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			}
			else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("Trying to send file message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageFile(receiver, sentTime, willQueue, file, coverImage, mimeType, fileName, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageFile(receiver, messageId, sentTime, file, coverImage, mimeType, fileName, caption);
			}

			return true;
		}

		bool SimpleMessageCenter::sendVideo(openmittsu::protocol::ContactId const& receiver, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("Trying to send video message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageVideo(receiver, sentTime, willQueue, video, coverImage, lengthInSeconds);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageVideo(receiver, messageId, sentTime, video, coverImage, lengthInSeconds);
			}

			return true;
		}

		bool SimpleMessageCenter::sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("Trying to send text message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageText(receiver, sentTime, willQueue, text);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageText(receiver, messageId, sentTime, text);
			}

			return true;
		}

		bool SimpleMessageCenter::sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
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
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageImage(receiver, sentTime, willQueue, imageBytes, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageImage(receiver, messageId, sentTime, imageBytes, caption);
			}

			return true;
		}

		bool SimpleMessageCenter::sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("Trying to send location message to unknown contact {}", receiver.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentContactMessageLocation(receiver, sentTime, willQueue, location);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentContactMessageLocation(receiver, messageId, sentTime, location);
			}

			return true;
		}

		void SimpleMessageCenter::sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) {
			if (!m_optionReader.getOptionAsBool(openmittsu::options::Options::BOOLEAN_SEND_TYPING_NOTIFICATION)) {
				return;
			}

			if (!this->m_storage.hasDatabase()) {
				return;
			} else if (!this->m_storage.hasContact(receiver)) {
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
				messageId = this->m_storage.storeSentContactMessageNotificationTypingStarted(receiver, sentTime, willQueue);
			} else {
				messageId = this->m_storage.storeSentContactMessageNotificationTypingStopped(receiver, sentTime, willQueue);
			}

			if (willQueue) {
				if (isTyping) {
					m_networkSentMessageAcceptor->processSentContactMessageTypingStarted(receiver, messageId, sentTime);
				} else {
					m_networkSentMessageAcceptor->processSentContactMessageTypingStopped(receiver, messageId, sentTime);
				}
			}
		}

		bool SimpleMessageCenter::sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasContact(receiver)) {
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
					messageId = m_storage.storeSentContactMessageReceiptReceived(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
					messageId = m_storage.storeSentContactMessageReceiptSeen(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE:
					messageId = m_storage.storeSentContactMessageReceiptAgree(receiver, sentTime, willQueue, receiptedMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE:
					messageId = m_storage.storeSentContactMessageReceiptDisagree(receiver, sentTime, willQueue, receiptedMessageId);
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

		bool SimpleMessageCenter::sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send receipt message to unknown group {}", group.toString());
				return false;
			}

			switch (receiptType) {
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
					//this->m_storage.store(group, receiptedopenmittsu::protocol::MessageId, openmittsu::protocol::MessageTime::now());
					break;
				default:
					LOGGER()->warn("Trying to send a receipt \"{}\" for message ID {} to group {}, this should never happen.", static_cast<int>(receiptType), receiptedMessageId.toString(), group.toString());
					return false;
			}

			return true;
		}

		bool SimpleMessageCenter::sendLeave(openmittsu::protocol::GroupId const& group) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() == group.getOwner()) {
				LOGGER()->warn("Trying to send leave message to group {} which is owned by us.", group.toString());
				return false;
			}
			
			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupLeave(group, sentTime, willQueue, true);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupLeave(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, this->m_storage.getSelfContact());
			}

			return true;
		}

		bool SimpleMessageCenter::sendSyncRequest(openmittsu::protocol::GroupId const& group) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() == group.getOwner()) {
				LOGGER()->warn("Trying to send sync request message to group {} which is owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupSyncRequest(group, sentTime, willQueue);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSyncRequest(group, { group.getOwner() }, messageId, sentTime);
			}

			return true;
		}

		bool SimpleMessageCenter::sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group creation message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupCreation(group, members, m_storage.getGroupMembers(group, true), true);
		}

		bool SimpleMessageCenter::sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group creation message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupCreation(group, sentTime, willQueue, members, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupCreation(group, recipients, messageId, sentTime, members);
			}

			return true;
		}
		
		bool SimpleMessageCenter::sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group title message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupTitle(group, title, m_storage.getGroupMembers(group, true), true);
		}

		bool SimpleMessageCenter::sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group title message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupSetTitle(group, sentTime, willQueue, title, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSetTitle(group, recipients, messageId, sentTime, title);
			}

			return true;
		}
		
		bool SimpleMessageCenter::sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group image message to group {} which is not owned by us.", group.toString());
				return false;
			}

			return this->sendGroupImage(group, image, m_storage.getGroupMembers(group, true), true);
		}

		bool SimpleMessageCenter::sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (this->m_storage.getSelfContact() != group.getOwner()) {
				LOGGER()->warn("Trying to send group image message to group {} which is not owned by us.", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupSetImage(group, sentTime, willQueue, image, applyOperationInDatabase);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupSetImage(group, recipients, messageId, sentTime, image);
			}

			return true;
		}

		bool SimpleMessageCenter::sendAudio(openmittsu::protocol::GroupId const& group, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send audio message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageAudio(group, sentTime, willQueue, audio, lengthInSeconds);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageAudio(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, audio, lengthInSeconds);
			}

			return true;
		}

		bool SimpleMessageCenter::sendFile(openmittsu::protocol::GroupId const& group, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			}
			else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send file message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageFile(group, sentTime, willQueue, file, coverImage, mimeType, fileName, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageFile(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, file, coverImage, mimeType, fileName, caption);
			}

			return true;
		}

		bool SimpleMessageCenter::sendVideo(openmittsu::protocol::GroupId const& group, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send video message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageVideo(group, sentTime, willQueue, video, coverImage, lengthInSeconds);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageVideo(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, video, coverImage, lengthInSeconds);
			}

			return true;
		}

		bool SimpleMessageCenter::sendText(openmittsu::protocol::GroupId const& group, QString const& text) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send text message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageText(group, sentTime, willQueue, text);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageText(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, text);
			}

			return true;
		}

		bool SimpleMessageCenter::sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
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
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageImage(group, sentTime, willQueue, imageBytes, caption);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageImage(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, imageBytes, caption);
			}

			return true;
		}

		bool SimpleMessageCenter::sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			} else if (!this->m_storage.hasGroup(group)) {
				LOGGER()->warn("Trying to send location message to unknown group {}", group.toString());
				return false;
			}

			bool willQueue = true;
			if ((this->m_networkSentMessageAcceptor == nullptr) || (!this->m_networkSentMessageAcceptor->isConnected())) {
				willQueue = false;
			}

			openmittsu::protocol::MessageTime const sentTime = openmittsu::protocol::MessageTime::now();
			openmittsu::protocol::MessageId const messageId = this->m_storage.storeSentGroupMessageLocation(group, sentTime, willQueue, location);

			if (willQueue) {
				m_networkSentMessageAcceptor->processSentGroupMessageLocation(group, this->m_storage.getGroupMembers(group, true), messageId, sentTime, location);
			}

			return true;
		}

		void SimpleMessageCenter::setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) {
			this->m_networkSentMessageAcceptor = newNetworkSentMessageAcceptor;
			if (m_networkSentMessageAcceptor) {
				OPENMITTSU_CONNECT(m_networkSentMessageAcceptor.get(), readyToAcceptMessages(), this, tryResendingMessagesToNetwork());
				tryResendingMessagesToNetwork();
			}
		}

		void SimpleMessageCenter::tryResendingMessagesToNetwork() {
			if ((this->m_networkSentMessageAcceptor != nullptr) && (this->m_networkSentMessageAcceptor->isConnected()) && (this->m_storage.hasDatabase())) {
				LOGGER()->info("Asking database to send all queued messges now...");
				this->m_storage.sendAllWaitingMessages(m_networkSentMessageAcceptor);
			}
		}

		/*
			Received Messages from Network
		*/
		void SimpleMessageCenter::processReceivedContactMessageAudio(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact audio message from sender {} with message ID #{} sent at {} with audio {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(audio.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact audio message from sender {} with message ID #{} sent at {} with audio {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(audio.toHex()).toStdString());
				return;
			}

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageAudio(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), audio, lengthInSeconds);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void SimpleMessageCenter::processReceivedContactMessageFile(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact file message from sender {} with message ID #{} sent at {} with file {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(file.toHex()).toStdString());
				return;
			}
			else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact file message from sender {} with message ID #{} sent at {} with file {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(file.toHex()).toStdString());
				return;
			}

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageFile(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), file, coverImage, mimeType, fileName, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void SimpleMessageCenter::processReceivedContactMessageVideo(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact video message from sender {} with message ID #{} sent at {} with video {} and cover image {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(video.toHex()).toStdString(), QString(coverImage.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact video message from sender {} with message ID #{} sent at {} with video {} and cover image {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(video.toHex()).toStdString(), QString(coverImage.toHex()).toStdString());
				return;
			}

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageVideo(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), video, coverImage, lengthInSeconds);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void SimpleMessageCenter::processReceivedContactMessageText(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QString const& message) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact text message from sender {} with message ID #{} sent at {} with text {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), message.toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact text message from sender {} with message ID #{} sent at {} with text {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), message.toStdString());
				return;
			}

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageText(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), message);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void SimpleMessageCenter::processReceivedContactMessageImage(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& image) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact image message from sender {} with message ID #{} sent at {} with image {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact image message from sender {} with message ID #{} sent at {} with image {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			}

			QString const caption = parseCaptionFromImage(image);

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageImage(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), image, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}
		
		void SimpleMessageCenter::processReceivedContactMessageLocation(openmittsu::messages::ReceivedMessageHeader const& messageHeader, openmittsu::utility::Location const& location) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact location message from sender {} with message ID #{} sent at {} with location {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), location.toString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a contact location message from sender {} with message ID #{} sent at {} with location {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), location.toString());
				return;
			}

			openTabForIncomingMessage(messageHeader.getSender());
			this->m_storage.storeReceivedContactMessageLocation(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), location);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			sendReceipt(messageHeader.getSender(), messageHeader.getMessageId(), openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED);
		}

		void SimpleMessageCenter::processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact message receipt type RECEIVED from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a contact message receipt type RECEIVED from sender {} with message ID #{} sent at {} for message ID #{}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type RECEIVED from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			this->m_storage.storeReceivedContactMessageReceiptReceived(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact message receipt type SEEN from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a contact message receipt type SEEN from sender {} with message ID #{} sent at {} for message ID #{}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type SEEN from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			this->m_storage.storeReceivedContactMessageReceiptSeen(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact message receipt type AGREE from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a contact message receipt type AGREE from sender {} with message ID #{} sent at {} for message ID #{}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type AGREE from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			openTabForIncomingMessage(sender);
			this->m_storage.storeReceivedContactMessageReceiptAgree(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a contact message receipt type DISAGREE from sender {} with message ID #{} sent at {} for message ID #{} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a contact message receipt type DISAGREE from sender {} with message ID #{} sent at {} for message ID #{}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
				return;
			}

			LOGGER_DEBUG("We received a contact message receipt type DISAGREE from sender {} with message ID #{} sent at {} for message ID #{}.", sender.toString(), messageId.toString(), timeSent.toString(), referredMessageId.toString());
			openTabForIncomingMessage(sender);
			this->m_storage.storeReceivedContactMessageReceiptDisagree(sender, messageId, timeSent, referredMessageId);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a typing start notification from sender {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a typing start notification from sender {} with message ID #{} sent at {}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			LOGGER_DEBUG("We received a typing start notification from sender {} with message ID #{} sent at {}.", sender.toString(), messageId.toString(), timeSent.toString());
			this->m_storage.storeReceivedContactTypingNotificationTyping(sender, messageId, timeSent);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a typing stop notification from sender {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			} else if (!this->m_storage.hasContact(sender)) {
				LOGGER()->warn("We received a typing stop notification from sender {} with message ID #{} sent at {}, but we do not recognize the sender. Ignoring.", sender.toString(), messageId.toString(), timeSent.toString());
				return;
			}

			LOGGER_DEBUG("We received a typing stop notification from sender {} with message ID #{} sent at {}.", sender.toString(), messageId.toString(), timeSent.toString());
			this->m_storage.storeReceivedContactTypingNotificationStopped(sender, messageId, timeSent);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(sender, messageId);
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageAudio(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group audio message from sender {} for group {} with message ID #{} sent at {} with audio {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(audio.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group audio message from sender {} for group {} with message ID #{} sent at {} with audio {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(audio.toHex()).toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP AUDIO message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::AUDIO, audio, lengthInSeconds));
				return;
			}

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageAudio(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), audio, lengthInSeconds);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageFile(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group file message from sender {} for group {} with message ID #{} sent at {} with file {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(file.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group file message from sender {} for group {} with message ID #{} sent at {} with file {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(file.toHex()).toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP FILE message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::FILE, file, coverImage, mimeType, fileName, caption));
				return;
			}

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageFile(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), file, coverImage, mimeType, fileName, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageVideo(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group video message from sender {} for group {} with message ID #{} sent at {} with video {} and cover image {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(video.toHex()).toStdString(), QString(coverImage.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group video message from sender {} for group {} with message ID #{} sent at {} with video {} and cover image {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(video.toHex()).toStdString(), QString(coverImage.toHex()).toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP VIDEO message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::VIDEO, video, coverImage, lengthInSeconds));
				return;
			}

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageVideo(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), video, coverImage, lengthInSeconds);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageText(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& message) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group text message from sender {} for group {} with message ID #{} sent at {} with text {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), message.toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group text message from sender {} for group {} with message ID #{} sent at {} with text {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), message.toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP TEXT message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::TEXT, message));
				return;
			}

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageText(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), message);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group image message from sender {} for group {} with message ID #{} sent at {} with image {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group image message from sender {} for group {} with message ID #{} sent at {} with image {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP IMAGE message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::IMAGE, image));
				return;
			}

			QString const caption = parseCaptionFromImage(image);

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageImage(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), image, caption);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupMessageLocation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, openmittsu::utility::Location const& location) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group location message from sender {} for group {} with message ID #{} sent at {} with location {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), location.toString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group location message from sender {} for group {} with message ID #{} sent at {} with location {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), location.toString());
				return;
			}

			LOGGER_DEBUG("Received GROUP LOCATION message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				QVariant locationVariant;
				locationVariant.setValue(location);
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::LOCATION, locationVariant));
				return;
			}

			openTabForIncomingMessage(messageHeader.getGroupId());
			this->m_storage.storeReceivedGroupMessageLocation(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), location);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}


		void SimpleMessageCenter::processReceivedGroupCreation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QSet<openmittsu::protocol::ContactId> const& members) {
			if (!this->m_storage.hasDatabase()) {
				QString const memberString = openmittsu::protocol::ContactIdList(members).toStringS();
				LOGGER()->warn("We received a group creation message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), memberString.toStdString());
				return;
			} else if (messageHeader.getSender() != messageHeader.getGroupId().getOwner()) {
				QString const memberString = openmittsu::protocol::ContactIdList(members).toStringS();
				LOGGER()->warn("We received a group creation message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), memberString.toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				QString const memberString = openmittsu::protocol::ContactIdList(members).toStringS();
				LOGGER()->warn("We received a group creation message from sender {} for group {} with message ID #{} sent at {} with members {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), memberString.toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP CREATION message from {} in group {} with ID {} and members {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), openmittsu::protocol::ContactIdList(members).toStringS().toStdString());

			QSet<openmittsu::protocol::ContactId> membersLocal(members);
			/*
			* Okay, so there is a bug/feature in e.g. the iOS client - it will not include itself in the group members, even though it considers itself part of the group.
			* Since the owner leaving the group is very uncommon, we will ignore this and just always add the owner...
			*/
			if (!membersLocal.contains(messageHeader.getGroupId().getOwner())) {
				LOGGER()->warn("Group creation for {} did not contain the owner; because of the iOS anomaly, we are adding him anyway. This might cause desync when sending messages.", messageHeader.getGroupId().toString());
				membersLocal.insert(messageHeader.getGroupId().getOwner());
			}

			this->m_storage.storeReceivedGroupCreation(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), membersLocal);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			QVector<MessageQueue::ReceivedGroupMessage> queuedMessages = m_messageQueue.getAndRemoveQueuedMessages(messageHeader.getGroupId());
			auto it = queuedMessages.constBegin();
			auto const end = queuedMessages.constEnd();
			for (; it != end; ++it) {
				messages::GroupMessageType const messageType = it->messageType;
				LOGGER_DEBUG("Processing message in Queue for group {} with type {}.", messageHeader.getGroupId().toString(), messages::GroupMessageTypeHelper::toString(messageType));
				switch (messageType) {
					case messages::GroupMessageType::AUDIO:
						processReceivedGroupMessageAudio(it->messageHeader, it->content.toByteArray(), it->contentTwo.toUInt());
						break;
					case messages::GroupMessageType::FILE:
						processReceivedGroupMessageFile(it->messageHeader, it->content.toByteArray(), it->contentTwo.toByteArray(), it->contentThree.toString(), it->contentFour.toString(), it->contentFive.toString());
						break;
					case messages::GroupMessageType::IMAGE:
						processReceivedGroupMessageImage(it->messageHeader, it->content.toByteArray());
						break;
					case messages::GroupMessageType::LEAVE:
						processReceivedGroupLeave(it->messageHeader);
						break;
					case messages::GroupMessageType::LOCATION:
						processReceivedGroupMessageLocation(it->messageHeader, it->content.value<openmittsu::utility::Location>());
						break;
					case messages::GroupMessageType::SET_IMAGE:
						processReceivedGroupSetImage(it->messageHeader, it->content.toByteArray());
						break;
					case messages::GroupMessageType::SET_TITLE:
						processReceivedGroupSetTitle(it->messageHeader, it->content.toString());
						break;
					case messages::GroupMessageType::SYNC_REQUEST:
						processReceivedGroupSyncRequest(it->messageHeader);
						break;
					case messages::GroupMessageType::TEXT:
						processReceivedGroupMessageText(it->messageHeader, it->content.toString());
						break;
					case messages::GroupMessageType::VIDEO:
						processReceivedGroupMessageVideo(it->messageHeader, it->content.toByteArray(), it->contentTwo.toByteArray(), it->contentThree.toUInt());
						break;
					default:
						throw openmittsu::exceptions::InternalErrorException() << "Group Message queue contains a message of type \"" << messages::GroupMessageTypeHelper::toString(messageType) << "\", which is unhandled. This should never happen!";
				}
			}
		}

		void SimpleMessageCenter::processReceivedGroupSetImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group set image message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			}
			
			// Since the protocol only includes the groups ID, not its owner, we do this silly dance:
			std::unique_ptr<openmittsu::protocol::GroupId> realGroupId;
			quint64 const partialGroupId = messageHeader.getGroupId().getGroupId();
			if (!m_messageQueue.hasGroupIdForPartial(partialGroupId, realGroupId)) {
				openmittsu::database::GroupToGroupDataMap groupData = m_storage.getGroupDataAll(false);
				for (auto it = groupData.constBegin(); it != groupData.constEnd(); ++it) {
					if (it.key().getGroupId() == partialGroupId) {
						realGroupId = std::make_unique<openmittsu::protocol::GroupId>(it.key());
						break;
					}
				}
			}
			
			if (!realGroupId) {
				// Since this is a protocol quirk, just drop the message and wait for a re-send
				LOGGER()->warn("We received a group set image message for group {} with message ID #{} sent at {} with members {} that we could not determine the group owner for (protocol quirk!). Ignoring.", messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			} else if (messageHeader.getSender() != realGroupId->getOwner()) {
				LOGGER()->warn("We received a group set image message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", messageHeader.getSender().toString(), realGroupId->toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group set image message from sender {} for group {} with message ID #{} sent at {} with members {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), realGroupId->toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), QString(image.toHex()).toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP SET_IMAGE message from {} in group {} with ID {}.", messageHeader.getSender().toString(), realGroupId->toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(*realGroupId, messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::SET_IMAGE, image));
				return;
			}

			this->m_storage.storeReceivedGroupSetImage(*realGroupId, messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), image);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupSetTitle(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& groupTitle) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} with members {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), groupTitle.toStdString());
				return;
			} else if (messageHeader.getSender() != messageHeader.getGroupId().getOwner()) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} with members {} that did not come from the group owner. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), groupTitle.toStdString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group set title message from sender {} for group {} with message ID #{} sent at {} with members {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString(), groupTitle.toStdString());
				return;
			}

			LOGGER_DEBUG("Received GROUP SET_TITLE message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::SET_TITLE, groupTitle));
				return;
			}

			this->m_storage.storeReceivedGroupSetTitle(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived(), groupTitle);
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processReceivedGroupSyncRequest(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			} else if (messageHeader.getSender() != messageHeader.getGroupId().getOwner()) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {} that did come from the group owner. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			} else if (messageHeader.getGroupId().getOwner() != m_storage.getSelfContact()) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {}, but we are not the group owner. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group sync request message from sender {} for group {} with message ID #{} sent at {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			}

			LOGGER_DEBUG("Received GROUP SYNC_REQUEST message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::SYNC_REQUEST, QVariant()));
				return;
			}

			this->m_storage.storeReceivedGroupSyncRequest(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived());
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}

			this->resendGroupSetup(messageHeader.getGroupId(), { messageHeader.getSender() });
		}

		void SimpleMessageCenter::processReceivedGroupLeave(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We received a group leave message from sender {} for group {} with message ID #{} sent at {} that could not be saved as the storage system is not ready.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			} else if (!this->m_storage.hasContact(messageHeader.getSender())) {
				LOGGER()->warn("We received a group leave message from sender {} for group {} with message ID #{} sent at {}, but we do not recognize the sender. Ignoring.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString(), messageHeader.getTimeSent().toString());
				return;
			}

			LOGGER_DEBUG("Received GROUP LEAVE message from {} in group {} with ID {}.", messageHeader.getSender().toString(), messageHeader.getGroupId().toString(), messageHeader.getMessageId().toString());
			if (!checkAndFixGroupMembership(messageHeader.getGroupId(), messageHeader.getSender())) {
				m_messageQueue.storeGroupMessage(MessageQueue::ReceivedGroupMessage(messageHeader, messages::GroupMessageType::LEAVE, QVariant()));
				return;
			}

			this->m_storage.storeReceivedGroupLeave(messageHeader.getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTimeSent(), messageHeader.getTimeReceived());
			if (this->m_networkSentMessageAcceptor != nullptr) {
				this->m_networkSentMessageAcceptor->sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
			}
		}

		void SimpleMessageCenter::processMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} failed, but that could not be saved as the storage system is not ready.", receiver.toString(), messageId.toString());
				return;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} failed, but we do not recognize the receiver. Ignoring.", receiver.toString(), messageId.toString());
				return;
			}
			this->m_storage.storeMessageSendFailed(receiver, messageId);
		}

		void SimpleMessageCenter::processMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} was successful, but that could not be saved as the storage system is not ready.", receiver.toString(), messageId.toString());
				return;
			} else if (!this->m_storage.hasContact(receiver)) {
				LOGGER()->warn("We were notfied that sending a message to user {} with message ID #{} was successful, but we do not recognize the receiver. Ignoring.", receiver.toString(), messageId.toString());
				return;
			}
			this->m_storage.storeMessageSendDone(receiver, messageId);
		}

		void SimpleMessageCenter::processMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were notfied that sending a message to group {} with message ID #{} failed, but that could not be saved as the storage system is not ready.", group.toString(), messageId.toString());
				return;
			}
			this->m_storage.storeMessageSendFailed(group, messageId);
		}

		void SimpleMessageCenter::processMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were notfied that sending a message to group {} with message ID #{} was successful, but that could not be saved as the storage system is not ready.", group.toString(), messageId.toString());
				return;
			}
			this->m_storage.storeMessageSendDone(group, messageId);
		}

		void SimpleMessageCenter::addNewContact(openmittsu::protocol::ContactId const& newContact, openmittsu::crypto::PublicKey const& publicKey) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were notfied of a new contact with ID {}, but that could not be saved as the storage system is not ready.", newContact.toString());
				return;
			}

			this->m_storage.storeNewContact(newContact, publicKey);
		}

		void SimpleMessageCenter::onFoundNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members) {
			if (!this->m_storage.hasDatabase()) {
				QString const memberString = openmittsu::protocol::ContactIdList(members).toStringS();
				LOGGER()->warn("We were notfied of a new group with ID {} and members {}, but that could not be saved as the storage system is not ready.", groupId.toString(), memberString.toStdString());
				return;
			}

			this->m_storage.storeNewGroup(groupId, members, false);
		}

		bool SimpleMessageCenter::createNewGroupAndInformMembers(QSet<openmittsu::protocol::ContactId> const& members, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) {
			if (!this->m_storage.hasDatabase()) {
				return false;
			}

			openmittsu::protocol::ContactId const selfContactId = m_storage.getSelfContact();
			openmittsu::protocol::GroupId groupId = openmittsu::protocol::GroupId::createRandomGroupId(selfContactId);
			while (m_storage.hasGroup(groupId)) {
				groupId = openmittsu::protocol::GroupId::createRandomGroupId(selfContactId);
			}

			QSet<openmittsu::protocol::ContactId> groupMembers = members;
			if (addSelfContact) {
				groupMembers.insert(selfContactId);
			}

			this->m_storage.storeNewGroup(groupId, groupMembers, false);

			sendGroupCreation(groupId, groupMembers);
			if (!groupTitle.isNull() && groupTitle.canConvert<QString>()) {
				sendGroupTitle(groupId, groupTitle.toString());
			}
			if (!groupImage.isNull() && groupImage.canConvert<QByteArray>()) {
				sendGroupImage(groupId, groupImage.toByteArray());
			}

			return true;
		}

		void SimpleMessageCenter::resendGroupSetup(openmittsu::protocol::GroupId const& group) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were asked to re-send the group setup for group {}, but that could not be done as the storage system is not ready.", group.toString());
				return;
			}

			resendGroupSetup(group, m_storage.getGroupMembers(group, true));
		}

		void SimpleMessageCenter::resendGroupSetup(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& recipients) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER()->warn("We were asked to re-send the group setup for group {}, but that could not be done as the storage system is not ready.", group.toString());
				return;
			}

			openmittsu::database::GroupData const groupData = m_storage.getGroupData(group, true);

			sendGroupCreation(group, groupData.members, recipients, false);
			sendGroupTitle(group, groupData.title, recipients, false);
			if (groupData.hasImage) {
				if (groupData.image.isAvailable()) {
					sendGroupImage(group, groupData.image.getData(), recipients, false);
				}
			}
		}

		QString SimpleMessageCenter::parseCaptionFromImage(QByteArray const& image) const {
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

		void SimpleMessageCenter::embedCaptionIntoImage(QByteArray& image, QString const& caption) const {
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

		void SimpleMessageCenter::openTabForIncomingMessage(openmittsu::protocol::ContactId const& contact) {
			emit newUnreadMessageAvailableContact(contact);
		}

		void SimpleMessageCenter::openTabForIncomingMessage(openmittsu::protocol::GroupId const& group) {
			emit newUnreadMessageAvailableGroup(group);
		}

		void SimpleMessageCenter::requestSyncForGroupIfApplicable(openmittsu::protocol::GroupId const& group) {
			if (!m_messageQueue.hasMessageForGroup(group)) {
				openmittsu::protocol::MessageTime const lastSyncRequestTime = this->m_storage.getGroupLastSyncRequestTime(group);
				bool isLastSyncRequestLongEnoughInThePast = false;
				if (lastSyncRequestTime.isNull()) {
					LOGGER_DEBUG("Last sync request time for group {} is null!", group.toString());
					isLastSyncRequestLongEnoughInThePast = true;
				} else if (lastSyncRequestTime.getTime().addSecs(5 * 60) < QDateTime::currentDateTime()) { // At least 5 minutes since the last sync request
					LOGGER_DEBUG("Last sync request time for group {} is {}!", group.toString(), lastSyncRequestTime.getTime().toString().toStdString());
					isLastSyncRequestLongEnoughInThePast = true;
				}

				if (isLastSyncRequestLongEnoughInThePast) {
					LOGGER_DEBUG("requestSyncForGroupIfApplicable for group {} will send sync request.", group.toString());
					this->sendSyncRequest(group);
				}
			} else {
				LOGGER_DEBUG("requestSyncForGroupIfApplicable for group {} will NOT send sync request, messageQueue has waiting messages for group.", group.toString());
			}
		}

		bool SimpleMessageCenter::checkAndFixGroupMembership(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender) {
			if (!this->m_storage.hasDatabase()) {
				LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {} failed, database not available.", group.toString(), sender.toString());
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

				if (!this->m_storage.hasGroup(group)) {
					LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: group unknown.", group.toString(), sender.toString());
					if (group.getOwner() != this->m_storage.getSelfContact()) {
						LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: requesting sync if applicable.", group.toString(), sender.toString());
						requestSyncForGroupIfApplicable(group);
					}

					if (m_optionReader.getOptionAsBool(openmittsu::options::Options::BOOLEAN_TRUST_OTHERS)) {
						QSet<openmittsu::protocol::ContactId> groupMembers;
						if (this->m_storage.isDeleteted(group)) {
							groupMembers = this->m_storage.getGroupMembers(group, false); // need not exclude, as the group is deleted we are not in there anyway
						}
						groupMembers.insert(this->m_storage.getSelfContact());
						groupMembers.insert(sender);
						this->m_storage.storeNewGroup(group, groupMembers, true);

						LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: trusting sender, established temporary group.", group.toString(), sender.toString());
						return true;
					} else {
						LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: not trusting others, rejecting.", group.toString(), sender.toString());
						return false;
					}
				} else {
					if (this->m_storage.getGroupMembers(group, false).contains(sender)) {
						return true;
					} else {
						if (group.getOwner() == this->m_storage.getSelfContact()) {
							LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: sender not in group, we are the owner, rejecting.", group.toString(), sender.toString());
							return false;
						} else if (group.getOwner() == sender) {
							LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: sender not in group, but it is the owner, accepting.", group.toString(), sender.toString());
							return true;
						} else {
							LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: sender not in group, requesting sync if applicable.", group.toString(), sender.toString());
							requestSyncForGroupIfApplicable(group);
							if (m_optionReader.getOptionAsBool(openmittsu::options::Options::BOOLEAN_TRUST_OTHERS)) {
								QSet<openmittsu::protocol::ContactId> groupMembers = this->m_storage.getGroupMembers(group, false);
								groupMembers.insert(this->m_storage.getSelfContact());
								groupMembers.insert(sender);
								this->m_storage.storeNewGroup(group, groupMembers, true);

								LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: trusting sender (group exists, sender not in group), established temporary group.", group.toString(), sender.toString());
								return true;
							} else {
								LOGGER_DEBUG("checkAndFixGroupMembership for group {} with sender {}: not trusting others (group exists, sender not in group), rejecting.", group.toString(), sender.toString());
								return false;
							}
						}
					}
				}
			}
		}

	}
}

