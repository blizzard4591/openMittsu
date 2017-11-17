#ifndef OPENMITTSU_PROTOCOLCLIENT_H_
#define OPENMITTSU_PROTOCOLCLIENT_H_

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>
#include <QMultiHash>
#include <QTimer>
#include <QMutex>
#include <QTcpSocket>
#include <QtNetwork>
#include <QDateTime>
#include <cstdint>
#include <utility>
#include <memory>

#include "src/crypto/KeyPair.h"
#include "src/crypto/PublicKey.h"
#include "src/network/ServerConfiguration.h"
#include "src/network/MessageCenterWrapper.h"
#include "src/utility/OptionMaster.h"
#include "src/network/MissingIdentityProcessor.h"

#include "src/acknowledgments/AcknowledgmentProcessor.h"

#include "src/crypto/FullCryptoBox.h"
#include "src/protocol/ContactIdWithMessageId.h"
#include "src/protocol/GroupRegistry.h"
#include "src/dataproviders/KeyRegistry.h"
#include "src/protocol/UniqueMessageIdGenerator.h"
#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/messages/MessageWithPayload.h"
#include "src/messages/contact/ContactMessage.h"
#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/messages/contact/ContactImageMessageContent.h"
#include "src/messages/contact/ContactTextMessageContent.h"
#include "src/messages/contact/ContactLocationMessageContent.h"
#include "src/messages/contact/ReceiptMessageContent.h"
#include "src/messages/contact/UserTypingMessageContent.h"
#include "src/messages/group/PreliminaryGroupMessage.h"
#include "src/messages/group/GroupTextMessageContent.h"
#include "src/messages/group/GroupImageMessageContent.h"
#include "src/messages/group/GroupLocationMessageContent.h"
#include "src/messages/group/GroupFileMessageContent.h"
#include "src/messages/group/GroupCreationMessageContent.h"
#include "src/messages/group/GroupSetPhotoMessageContent.h"
#include "src/messages/group/GroupSetTitleMessageContent.h"
#include "src/messages/group/GroupSyncMessageContent.h"
#include "src/messages/group/GroupLeaveMessageContent.h"
#include "src/messages/group/SpecializedGroupMessage.h"
#include "src/messages/group/UnspecializedGroupMessage.h"

namespace openmittsu {
	namespace network {

		class ProtocolClient : public QObject {
			Q_OBJECT

		public:
			ProtocolClient(std::shared_ptr<openmittsu::crypto::FullCryptoBox> cryptoBox, openmittsu::protocol::ContactId const& ourContactId, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster, std::shared_ptr<openmittsu::network::MessageCenterWrapper> const& messageCenterWrapper, openmittsu::protocol::PushFromId const& pushFromId);
			virtual ~ProtocolClient();
			bool getIsConnected() const;

			friend class openmittsu::acknowledgments::AcknowledgmentProcessor;
		public slots:
			void setup();
			void teardown();
			void connectToServer();
			void disconnectFromServer();
			void sendContactMessage(openmittsu::messages::contact::PreliminaryContactMessage const& preliminaryMessage);
			void sendGroupMessage(openmittsu::messages::group::PreliminaryGroupMessage const& preliminaryMessage);
			void sendMessageReceivedAcknowledgement(openmittsu::protocol::ContactId const& messageSender, openmittsu::protocol::MessageId const& messageId);

			void newPushFromId(openmittsu::protocol::PushFromId const& newPushFromId);

			quint64 getReceivedMessagesCount() const;
			quint64 getSendMessagesCount() const;
			quint64 getReceivedBytesCount() const;
			quint64 getSendBytesCount() const;
			QDateTime const& getConnectedSince() const;
		signals:
			void setupDone();
			void teardownComplete();
			void connectToFinished(int errCode, QString message);
			void readyConnect();

			void groupSetupDone(openmittsu::protocol::GroupId const& groupId, bool successfull);

			void duplicateIdUsageDetected();
			void lostConnection();
			private slots:
			void socketOnReadyRead();
			void socketOnError(QAbstractSocket::SocketError socketError);
			void socketConnected();
			void socketDisconnected(bool emitSignal = true);
			void networkSessionOnIsOpen();
			void outgoingMessagesTimerOnTimer();
			void acknowledgmentWaitingTimerOnTimer();
			void keepAliveTimerOnTimer();
			void callbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask);
		private:
			std::shared_ptr<openmittsu::crypto::FullCryptoBox> m_cryptoBox;
			std::shared_ptr<openmittsu::network::MessageCenterWrapper> const m_messageCenterWrapper;
			std::unique_ptr<openmittsu::protocol::PushFromId> m_pushFromIdPtr;

			bool m_isSetupDone;
			bool m_isNetworkSessionReady;
			bool m_isConnected;
			bool m_isAllowedToSend;
			bool m_isDisconnecting;
			QByteArray m_readyReadRemainingData;
			std::unique_ptr<QTcpSocket> m_socket;
			std::unique_ptr<QNetworkSession> m_networkSession;
			openmittsu::protocol::ContactId const m_ourContactId;
			std::shared_ptr<openmittsu::network::ServerConfiguration> m_serverConfiguration;
			std::shared_ptr<openmittsu::utility::OptionMaster> m_optionMaster;

			// Outgoing Message List
			QList<QByteArray> outgoingMessages;
			std::unique_ptr<QTimer> outgoingMessagesTimer;
			QMutex outgoingMessagesMutex;

			// List of Messages to be acknowledged by the server
			QHash<openmittsu::protocol::MessageId, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>> acknowledgmentWaitingMessages;

			std::unique_ptr<QTimer> acknowledgmentWaitingTimer;
			QMutex acknowledgmentWaitingMutex;

			// List of Messages kept back because we are waiting for IdentityReceivers
			QHash<openmittsu::protocol::ContactId, std::shared_ptr<MissingIdentityProcessor>> missingIdentityProcessors;
			QHash<openmittsu::protocol::GroupId, std::shared_ptr<MissingIdentityProcessor>> groupsWithMissingIdentities;

			// Connection Keep-Alive
			std::unique_ptr<QTimer> keepAliveTimer;
			uint32_t keepAliveCounter;
			quint64 failedReconnectAttempts;

			// Statistics
			quint64 messagesReceived;
			quint64 messagesSend;
			quint64 bytesSend;
			quint64 bytesReceived;
			QDateTime connectionStart;

			void enqeueCallbackTask(openmittsu::tasks::CallbackTask* callbackTask);
			void enqeueWaitForAcknowledgment(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, std::shared_ptr < openmittsu::acknowledgments::AcknowledgmentProcessor > const& acknowledgmentProcessor);
			bool waitForData(qint64 minBytesRequired);
			void sendClientAcknowlegmentForMessage(openmittsu::messages::MessageWithEncryptedPayload const& message);
			void handleIncomingAcknowledgment(openmittsu::protocol::MessageId const& messageId);

			void messageSendFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId);
			void messageSendFailed(openmittsu::protocol::ContactId const& contactId, openmittsu::protocol::MessageId const& messageId);
			void messageSendDone(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId);
			void messageSendDone(openmittsu::protocol::ContactId const& contactId, openmittsu::protocol::MessageId const& messageId);

			void handleIncomingMessage(openmittsu::messages::MessageWithEncryptedPayload const& message);
			void handleIncomingMessage(openmittsu::messages::MessageWithPayload const& messageWithPayload, openmittsu::messages::MessageWithEncryptedPayload const*const message);
			void handleIncomingMessage(openmittsu::messages::Message const*const message, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactTextMessageContent const> contactTextMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactImageMessageContent const> contactImageMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactLocationMessageContent const> contactLocationMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ReceiptMessageContent const> receiptMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::UserTypingMessageContent const> userTypingMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupTextMessageContent const> groupTextMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupImageMessageContent const> groupImageMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupLocationMessageContent const> groupLocationMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupFileMessageContent const> groupFileMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupCreationMessageContent const> groupCreationMessageContent, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSetPhotoMessageContent const> groupSetPhotoMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSetTitleMessageContent const> groupSetTitleMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSyncMessageContent const> groupSyncMessageContent);
			void handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupLeaveMessageContent const> groupLeaveMessageContent);
			void handleOutgoingMessage(openmittsu::messages::contact::ContactMessage const*const contactMessage, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor);
			void handleOutgoingMessage(openmittsu::messages::group::UnspecializedGroupMessage const*const message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor);
			void handleOutgoingMessage(openmittsu::messages::group::SpecializedGroupMessage const*const message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor);
			void encryptAndSendDataPacketToServer(QByteArray const& dataPacket);
			void handleIncomingKeepAliveRequest(QByteArray const& packetData);
			void handleIncomingKeepAliveAnswer(QByteArray const& packetData);

			// Groups
			bool needToWaitForMissingIdentity(openmittsu::protocol::ContactId const& contactId, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload);
			bool needToWaitForGroupData(openmittsu::protocol::GroupId const& groupId, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload, bool isGroupCreationMessage);
		};

	}
}

#endif // OPENMITTSU_PROTOCOLCLIENT_H_