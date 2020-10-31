#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_

#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/MessageCenterPointerAuthority.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenterWrapper : public MessageCenter {
			Q_OBJECT
		public:
			MessageCenterWrapper(MessageCenterPointerAuthority const* messageCenterPointerAuthority);
			MessageCenterWrapper(MessageCenterWrapper const& other);
			virtual ~MessageCenterWrapper();

			bool hasMessageCenter() const;
		public:
			virtual bool sendAudio(openmittsu::protocol::ContactId const& receiver, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual bool sendFile(openmittsu::protocol::ContactId const& receiver, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual bool sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) override;
			virtual bool sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) override;
			virtual bool sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) override;
			virtual bool sendVideo(openmittsu::protocol::ContactId const& receiver, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual bool sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) override;
			virtual void sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) override;
			virtual bool sendAudio(openmittsu::protocol::GroupId const& group, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual bool sendFile(openmittsu::protocol::GroupId const& group, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual bool sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) override;
			virtual bool sendLeave(openmittsu::protocol::GroupId const& group) override;
			virtual bool sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) override;
			virtual bool sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) override;
			virtual bool sendSyncRequest(openmittsu::protocol::GroupId const& group) override;
			virtual bool sendText(openmittsu::protocol::GroupId const& group, QString const& text) override;
			virtual bool sendVideo(openmittsu::protocol::GroupId const& group, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) override;
			virtual bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) override;
			virtual void setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) override;
			virtual void processMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void processMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) override;
			virtual void processMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;
			virtual void processMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) override;
			virtual void processReceivedContactMessageAudio(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void processReceivedContactMessageFile(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void processReceivedContactMessageImage(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& image) override;
			virtual void processReceivedContactMessageLocation(openmittsu::messages::ReceivedMessageHeader const& messageHeader, openmittsu::utility::Location const& location) override;
			virtual void processReceivedContactMessageText(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QString const& message) override;
			virtual void processReceivedContactMessageVideo(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual void processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void processReceivedGroupMessageAudio(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void processReceivedGroupMessageFile(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void processReceivedGroupMessageImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) override;
			virtual void processReceivedGroupMessageLocation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, openmittsu::utility::Location const& location) override;
			virtual void processReceivedGroupMessageText(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& message) override;
			virtual void processReceivedGroupMessageVideo(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;
			virtual void processReceivedGroupCreation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual void processReceivedGroupSetImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) override;
			virtual void processReceivedGroupSetTitle(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& groupTitle) override;
			virtual void processReceivedGroupSyncRequest(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) override;
			virtual void processReceivedGroupLeave(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) override;
			virtual void addNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) override;
			virtual void resendGroupSetup(openmittsu::protocol::GroupId const& group) override;
			virtual bool createNewGroupAndInformMembers(QSet<openmittsu::protocol::ContactId> const& members, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) override;
		private slots:
			void onMessageCenterPointerAuthorityHasNewMessageCenter();
			void onNewUnreadMessageAvailableContact(openmittsu::protocol::ContactId const& contact);
			void onNewUnreadMessageAvailableGroup(openmittsu::protocol::GroupId const& group);
			void onMessageChanged(QString const& uuid);
			void onMessageDeleted(QString const& uuid);
		private:
			MessageCenterPointerAuthority const* m_messageCenterPointerAuthority;
			std::weak_ptr<MessageCenter> m_messageCenter;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_
