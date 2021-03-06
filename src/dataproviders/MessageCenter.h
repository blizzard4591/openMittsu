#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_

#include <QString>
#include <QObject>

#include "src/utility/Location.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/crypto/PublicKey.h"

#include "src/dataproviders/NetworkSentMessageAcceptor.h"
#include "src/protocol/GroupId.h"
#include "src/messages/ReceivedMessageHeader.h"
#include "src/messages/ReceivedGroupMessageHeader.h"
#include "src/messages/contact/ReceiptMessageContent.h"

namespace openmittsu {
	namespace widgets {
		class ChatTab;
	}

	namespace dataproviders {
		class MessageCenter : public QObject {
			Q_OBJECT
		public:
			virtual ~MessageCenter() {}
		signals:
			void newUnreadMessageAvailableContact(openmittsu::protocol::ContactId const& contact);
			void newUnreadMessageAvailableGroup(openmittsu::protocol::GroupId const& group);
			void messageChanged(QString const& uuid);
			void messageDeleted(QString const& uuid);
		public slots:
			virtual bool sendAudio(openmittsu::protocol::ContactId const& receiver, QByteArray const& audio, quint16 lengthInSeconds) = 0;
			virtual bool sendFile(openmittsu::protocol::ContactId const& receiver, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) = 0;
			virtual bool sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) = 0;
			virtual bool sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) = 0;
			virtual bool sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) = 0;
			virtual bool sendVideo(openmittsu::protocol::ContactId const& receiver, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) = 0;
		
			virtual bool sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) = 0;

			virtual void sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) = 0;

			virtual bool sendAudio(openmittsu::protocol::GroupId const& group, QByteArray const& audio, quint16 lengthInSeconds) = 0;
			virtual bool sendFile(openmittsu::protocol::GroupId const& group, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) = 0;
			virtual bool sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) = 0;
			virtual bool sendLeave(openmittsu::protocol::GroupId const& group) = 0;
			virtual bool sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) = 0;
			virtual bool sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) = 0;
			virtual bool sendSyncRequest(openmittsu::protocol::GroupId const& group) = 0;
			virtual bool sendText(openmittsu::protocol::GroupId const& group, QString const& text) = 0;
			virtual bool sendVideo(openmittsu::protocol::GroupId const& group, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) = 0;

			virtual bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) = 0;
			virtual bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) = 0;

			virtual void setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) = 0;

			virtual void processMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void processMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void processMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void processMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) = 0;

			virtual void processReceivedContactMessageAudio(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) = 0;
			virtual void processReceivedContactMessageFile(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) = 0;
			virtual void processReceivedContactMessageImage(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& image) = 0;
			virtual void processReceivedContactMessageLocation(openmittsu::messages::ReceivedMessageHeader const& messageHeader, openmittsu::utility::Location const& location) = 0;
			virtual void processReceivedContactMessageText(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QString const& message) = 0;
			virtual void processReceivedContactMessageVideo(openmittsu::messages::ReceivedMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) = 0;

			virtual void processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;

			virtual void processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;
			virtual void processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;

			virtual void processReceivedGroupMessageAudio(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& audio, quint16 lengthInSeconds) = 0;
			virtual void processReceivedGroupMessageFile(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) = 0;
			virtual void processReceivedGroupMessageImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) = 0;
			virtual void processReceivedGroupMessageLocation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, openmittsu::utility::Location const& location) = 0;
			virtual void processReceivedGroupMessageText(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& message) = 0;
			virtual void processReceivedGroupMessageVideo(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) = 0;

			virtual void processReceivedGroupCreation(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual void processReceivedGroupSetImage(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QByteArray const& image) = 0;
			virtual void processReceivedGroupSetTitle(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader, QString const& groupTitle) = 0;
			virtual void processReceivedGroupSyncRequest(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) = 0;
			virtual void processReceivedGroupLeave(openmittsu::messages::ReceivedGroupMessageHeader const& messageHeader) = 0;

			virtual void addNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) = 0;

			virtual void resendGroupSetup(openmittsu::protocol::GroupId const& group) = 0;
			virtual bool createNewGroupAndInformMembers(QSet<openmittsu::protocol::ContactId> const& members, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage) = 0;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
