#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_

#include <QObject>
#include <QString>
#include <QHash>
#include <QSet>
#include <QByteArray>
#include <QSharedPointer>

#include <cstdint>
#include <memory>

#include "src/utility/Location.h"
#include "src/utility/OptionMaster.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/widgets/TabController.h"

#include "src/dataproviders/MessageStorage.h"
#include "src/dataproviders/MessageQueue.h"
#include "src/dataproviders/ReceivedMessageAcceptor.h"
#include "src/dataproviders/NetworkSentMessageAcceptor.h"

#include "src/messages/contact/ReceiptMessageContent.h"

namespace openmittsu {
	namespace database {
		class Database;
	}

	namespace widgets {
		class ChatTab;
	}

	namespace dataproviders {
		class MessageStorage;

		class MessageCenter : public QObject, public ReceivedMessageAcceptor {
			Q_OBJECT
		public:
			MessageCenter(std::shared_ptr<openmittsu::widgets::TabController> const& tabController, std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster);
			virtual ~MessageCenter();

			bool sendText(openmittsu::protocol::ContactId const& receiver, QString const& text);
			bool sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption);
			bool sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location);
			bool sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType);

			void sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping);

			bool sendText(openmittsu::protocol::GroupId const& group, QString const& text);
			bool sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption);
			bool sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location);
			bool sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType);
			bool sendLeave(openmittsu::protocol::GroupId const& group);
			bool sendSyncRequest(openmittsu::protocol::GroupId const& group);

			bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members);
			bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title);
			bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image);

			void setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor);
			void setStorage(std::shared_ptr<openmittsu::dataproviders::MessageStorage> const& newStorage);
		signals:
			void newUnreadMessageAvailable(openmittsu::widgets::ChatTab* source);

			void messageChanged(QString const& uuid);
		public slots:
			virtual void processReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void processReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) override;
			virtual void processReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;

			virtual void processReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;

			virtual void processReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void processReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;

			virtual void processReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) override;
			virtual void processReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) override;
			virtual void processReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) override;

			virtual void processReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual void processReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) override;
			virtual void processReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& groupTitle) override;
			virtual void processReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;
			virtual void processReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) override;

			void onMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId);
			void onMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId);
			void onMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId);
			void onMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId);

			// New Contact, new Group, group changes...
			void onFoundNewContact(openmittsu::protocol::ContactId const& newContact, openmittsu::crypto::PublicKey const& publicKey);
			void onFoundNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members);

			bool createNewGroupAndInformMembers(QSet<openmittsu::protocol::ContactId> const& members, bool addSelfContact, QVariant const& groupTitle, QVariant const& groupImage);

			void resendGroupSetup(openmittsu::protocol::GroupId const& group);
			void resendGroupSetup(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& recipients);

			void databaseOnMessageChanged(QString const& uuid);
			void databaseOnContactHasNewMessage(openmittsu::protocol::ContactId const& identity, QString const& messageUuid);
			void databaseOnGroupHasNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid);
			void tryResendingMessagesToNetwork();
		private:
			MessageCenter(const MessageCenter &); // hide copy constructor
			MessageCenter& operator=(const MessageCenter &); // hide assign op
															 // we leave just the declarations, so the compiler will warn us 
															 // if we try to use those two functions by accident
			std::shared_ptr<openmittsu::widgets::TabController> const m_tabController;
			std::shared_ptr<openmittsu::utility::OptionMaster> const m_optionMaster;
			std::shared_ptr<NetworkSentMessageAcceptor> m_networkSentMessageAcceptor;
			std::shared_ptr<openmittsu::dataproviders::MessageStorage> m_storage;
			MessageQueue m_messageQueue;

			void openTabForIncomingMessage(openmittsu::protocol::ContactId const& sender);
			void openTabForIncomingMessage(openmittsu::protocol::GroupId const& group);

			bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase);
			bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase);
			bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QSet<openmittsu::protocol::ContactId> const& recipients, bool applyOperationInDatabase);

			bool checkAndFixGroupMembership(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender);
			void requestSyncForGroupIfApplicable(openmittsu::protocol::GroupId const& group);

			QString parseCaptionFromImage(QByteArray const& image) const;
			void embedCaptionIntoImage(QByteArray& image, QString const& caption) const;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
