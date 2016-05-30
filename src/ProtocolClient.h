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

#include "KeyPair.h"
#include "PublicKey.h"
#include "ServerConfiguration.h"
#include "ClientConfiguration.h"
#include "MissingIdentityProcessor.h"

#include "acknowledgments/AcknowledgmentProcessor.h"

#include "protocol/CryptoBox.h"
#include "protocol/ContactIdWithMessageId.h"
#include "protocol/GroupRegistry.h"
#include "protocol/KeyRegistry.h"
#include "protocol/UniqueMessageIdGenerator.h"
#include "messages/MessageWithEncryptedPayload.h"
#include "messages/MessageWithPayload.h"
#include "messages/contact/ContactMessage.h"
#include "messages/contact/PreliminaryContactMessage.h"
#include "messages/contact/ContactImageMessageContent.h"
#include "messages/contact/ContactTextMessageContent.h"
#include "messages/contact/ContactLocationMessageContent.h"
#include "messages/contact/ReceiptMessageContent.h"
#include "messages/contact/UserTypingMessageContent.h"
#include "messages/group/PreliminaryGroupMessage.h"
#include "messages/group/GroupTextMessageContent.h"
#include "messages/group/GroupImageMessageContent.h"
#include "messages/group/GroupLocationMessageContent.h"
#include "messages/group/GroupFileMessageContent.h"
#include "messages/group/GroupCreationMessageContent.h"
#include "messages/group/GroupSetPhotoMessageContent.h"
#include "messages/group/GroupSetTitleMessageContent.h"
#include "messages/group/GroupSyncMessageContent.h"
#include "messages/group/GroupLeaveMessageContent.h"
#include "messages/group/SpecializedGroupMessage.h"
#include "messages/group/UnspecializedGroupMessage.h"

class MessageCenter;

class ProtocolClient : public QObject {
	Q_OBJECT

public:
	ProtocolClient(KeyRegistry const& keyRegistry, GroupRegistry const& groupRegistry, UniqueMessageIdGenerator* messageIdGenerator, ServerConfiguration const& serverConfiguration, ClientConfiguration const& clientConfiguration, MessageCenter* messageCenter, PushFromId const& pushFromId);
	virtual ~ProtocolClient();
	bool getIsConnected() const;

	friend class AcknowledgmentProcessor;
public slots:
	void setup();
	void teardown();
	void connectToServer();
	void disconnectFromServer();
	void sendContactMessage(PreliminaryContactMessage const& preliminaryMessage);
	void sendGroupMessage(PreliminaryGroupMessage const& preliminaryMessage);

	void addContact(ContactId const& contactId, PublicKey const& publicKey);
	void newPushFromId(PushFromId const& newPushFromId);

	void sendGroupSetup(GroupId const& groupId, QSet<ContactId> const& members, QString const& title);
	void resendGroupSetup(GroupId const& groupId);
	void requestGroupSync(GroupId const& groupId);

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

	void groupSetupDone(GroupId const& groupId, bool successfull);

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
	void callbackTaskFinished(CallbackTask* callbackTask);
private:
	CryptoBox cryptoBox;
	GroupRegistry groupRegistry;
	UniqueMessageIdGenerator* uniqueMessageIdGenerator;
	MessageCenter* messageCenter;
	std::unique_ptr<PushFromId> pushFromIdPtr;

	bool isSetupDone;
	bool isNetworkSessionReady;
	bool isConnected;
	bool isAllowedToSend;
	QByteArray readyReadRemainingData;
	QTcpSocket* socket;
	QNetworkSession *networkSession;
	ServerConfiguration serverConfiguration;
	ClientConfiguration clientConfiguration;

	// Outgoing Message List
	QList<QByteArray> outgoingMessages;
	QTimer* outgoingMessagesTimer;
	QMutex outgoingMessagesMutex;

	// List of Messages to be acknowledged by the server
	QHash<MessageId, std::shared_ptr<AcknowledgmentProcessor>> acknowledgmentWaitingMessages;

	QTimer* acknowledgmentWaitingTimer;
	QMutex acknowledgmentWaitingMutex;

	// List of Messages kept back because we are waiting for IdentityReceivers
	QHash<ContactId, std::shared_ptr<MissingIdentityProcessor>> missingIdentityProcessors;
	QHash<GroupId, std::shared_ptr<MissingIdentityProcessor>> groupsWithMissingIdentities;
	QHash<GroupId, std::list<MessageWithEncryptedPayload>> groupsWaitingForSync;

	// Connection Keep-Alive
	QTimer* keepAliveTimer;
	uint32_t keepAliveCounter;

	// Statistics
	quint64 messagesReceived;
	quint64 messagesSend;
	quint64 bytesSend;
	quint64 bytesReceived;
	QDateTime connectionStart;

	void enqeueCallbackTask(CallbackTask* callbackTask);
	void enqeueWaitForAcknowledgment(MessageId const& messageId, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor);
	bool waitForData(qint64 minBytesRequired);
	void sendClientAcknowlegmentForMessage(MessageWithEncryptedPayload const& message);
	void handleIncomingAcknowledgment(ContactId const& sender, MessageId const& messageId);

	void messageSendFailed(GroupId const& groupId, MessageId const& messageId);
	void messageSendFailed(ContactId const& contactId, MessageId const& messageId);
	void messageSendDone(GroupId const& groupId, MessageId const& messageId);
	void messageSendDone(ContactId const& contactId, MessageId const& messageId);

	void handleIncomingMessage(MessageWithEncryptedPayload const& message);
	void handleIncomingMessage(MessageWithPayload const& messageWithPayload, MessageWithEncryptedPayload const*const message);
	void handleIncomingMessage(Message const*const message, MessageWithEncryptedPayload const*const messageWithEncryptedPayload);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactTextMessageContent const> contactTextMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactImageMessageContent const> contactImageMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactLocationMessageContent const> contactLocationMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ReceiptMessageContent const> receiptMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<UserTypingMessageContent const> userTypingMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupTextMessageContent const> groupTextMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupImageMessageContent const> groupImageMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupLocationMessageContent const> groupLocationMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupFileMessageContent const> groupFileMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupCreationMessageContent const> groupCreationMessageContent, MessageWithEncryptedPayload const*const messageWithEncryptedPayload);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSetPhotoMessageContent const> groupSetPhotoMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSetTitleMessageContent const> groupSetTitleMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSyncMessageContent const> groupSyncMessageContent);
	void handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupLeaveMessageContent const> groupLeaveMessageContent);
	void handleOutgoingMessage(ContactMessage const*const contactMessage, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor);
	void handleOutgoingMessage(UnspecializedGroupMessage const*const message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor);
	void handleOutgoingMessage(SpecializedGroupMessage const*const message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor);
	void encryptAndSendDataPacketToServer(QByteArray const& dataPacket);
	void handleIncomingKeepAliveRequest(QByteArray const& packetData);
	void handleIncomingKeepAliveAnswer(QByteArray const& packetData);

	// Groups
	bool validateGroupMembershipAndInfo(GroupId const& groupId, ContactId const& sender);
	void sendGroupSyncRequest(GroupId const& groupId);
};

#endif // OPENMITTSU_PROTOCOLCLIENT_H_