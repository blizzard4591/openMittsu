#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_

#include <QString>
#include <QObject>

#include "src/utility/Location.h"
#include "src/utility/OptionMaster.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/crypto/PublicKey.h"

#include "src/dataproviders/NetworkSentMessageAcceptor.h"
#include "src/protocol/GroupId.h"
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
			void newUnreadMessageAvailable(openmittsu::widgets::ChatTab* source);
			void messageChanged(QString const& uuid);
		public slots:
			virtual bool sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) = 0;
			virtual bool sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) = 0;
			virtual bool sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) = 0;
			virtual bool sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) = 0;

			virtual void sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) = 0;

			virtual bool sendText(openmittsu::protocol::GroupId const& group, QString const& text) = 0;
			virtual bool sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) = 0;
			virtual bool sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) = 0;
			virtual bool sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) = 0;
			virtual bool sendLeave(openmittsu::protocol::GroupId const& group) = 0;
			virtual bool sendSyncRequest(openmittsu::protocol::GroupId const& group) = 0;

			virtual bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) = 0;
			virtual bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) = 0;

			virtual void setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) = 0;
			virtual void setStorage(std::shared_ptr<openmittsu::database::Database> const& newStorage) = 0;

			virtual void processReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) = 0;
			virtual void processReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) = 0;
			virtual void processReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) = 0;

			virtual void processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;
			virtual void processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) = 0;

			virtual void processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;
			virtual void processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) = 0;

			virtual void processReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) = 0;
			virtual void processReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) = 0;
			virtual void processReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) = 0;

			virtual void processReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) = 0;
			virtual void processReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) = 0;
			virtual void processReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) = 0;
			virtual void processReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) = 0;
			virtual void processReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) = 0;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
