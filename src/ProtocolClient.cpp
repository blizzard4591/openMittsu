#include "ProtocolClient.h"

#include <QMessageBox>
#include <QMutex>
#include <QByteArray>

#include "acknowledgments/ContactMessageAcknowledgmentProcessor.h"
#include "acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"
#include "acknowledgments/GroupCreationMessageAcknowledgmentProcessor.h"
#include "exceptions/InternalErrorException.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/ProtocolErrorException.h"
#include "tasks/CallbackTask.h"
#include "tasks/IdentityReceiverCallbackTask.h"
#include "tasks/MessageCallbackTask.h"
#include "utility/ByteArrayConversions.h"
#include "utility/ByteArrayToHexString.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"

#include "messages/Message.h"
#include "messages/MessageFlagsFactory.h"
#include "messages/MessageWithEncryptedPayload.h"
#include "messages/MessageWithPayload.h"
#include "messages/IncomingMessagesParser.h"
#include "messages/contact/ContactMessage.h"
#include "messages/contact/ContactImageIdAndKeyMessageContent.h"
#include "messages/contact/ContactImageMessageContent.h"
#include "messages/group/SpecializedGroupMessage.h"
#include "messages/group/UnspecializedGroupMessage.h"
#include "messages/group/GroupTextMessageContent.h"
#include "messages/group/GroupImageMessageContent.h"
#include "protocol/AuthenticationPacket.h"
#include "protocol/ClientAcknowledgement.h"
#include "protocol/CryptoBox.h"
#include "protocol/NonceGenerator.h"
#include "ContactRegistry.h"
#include "Endian.h"
#include "IdentityHelper.h"
#include "MessageCenter.h"

#include "sodium.h"

ProtocolClient::ProtocolClient(KeyRegistry const& keyRegistry, GroupRegistry const& groupRegistry, UniqueMessageIdGenerator* messageIdGenerator, ServerConfiguration const& serverConfiguration, ClientConfiguration const& clientConfiguration, MessageCenter* messageCenter, PushFromId const& pushFromId)
	: QObject(nullptr), cryptoBox(keyRegistry), groupRegistry(groupRegistry), uniqueMessageIdGenerator(messageIdGenerator), messageCenter(messageCenter), pushFromIdPtr(std::unique_ptr<PushFromId>(new PushFromId(pushFromId))), isSetupDone(false), isNetworkSessionReady(false), isConnected(false), isAllowedToSend(false), socket(nullptr), networkSession(nullptr), serverConfiguration(serverConfiguration), clientConfiguration(clientConfiguration), outgoingMessagesTimer(nullptr), acknowledgmentWaitingTimer(nullptr), keepAliveTimer(nullptr), keepAliveCounter(0) {
	// Intentionally left empty.
}

ProtocolClient::~ProtocolClient() {
	if (isConnected || isSetupDone) {
		try {
			QEventLoop loop;
			OPENMITTSU_CONNECT(this, teardownComplete(), &loop, quit());
			QMetaObject::invokeMethod(this, "teardown", Qt::QueuedConnection);
			loop.exec();
		} catch (...) {
			LOGGER()->critical("Caught exception in ProtocolClient dtor, this will probably crash with an exception when auto-destruction takes over.");
		}
	}
}

void ProtocolClient::connectToServer() {
	if (!isSetupDone) {
		throw InternalErrorException() << "ProtocolClient::connect() was called before the initial setup was finished!";
	} else if (!isNetworkSessionReady) {
		throw InternalErrorException() << "ProtocolClient::connect() was called before the network session was available!";
	} else if (isConnected) {
		LOGGER()->info("Disconnecting from old server before connection with new settings...");
		disconnectFromServer();
	}
	
	// Clear socket
	socket->abort();

	// Reset stats
	messagesReceived = 0;
	messagesSend = 0;
	bytesSend = 0;
	bytesReceived = 0;
	acknowledgmentWaitingMutex.lock();
	acknowledgmentWaitingMessages.clear();
	acknowledgmentWaitingMutex.unlock();

	LOGGER()->info("Now connecting to {} on port {}.", serverConfiguration.getServerHost().toStdString(), serverConfiguration.getServerPort());
	socket->connectToHost(serverConfiguration.getServerHost(), serverConfiguration.getServerPort());
}

void ProtocolClient::socketDisconnected(bool emitSignal) {
	LOGGER()->info("Socket is now in state disconnected.");
	isConnected = false;
	isAllowedToSend = false;
	keepAliveTimer->stop();
	outgoingMessagesTimer->stop();

	if (emitSignal) {
		emit lostConnection();
	}
}

bool ProtocolClient::getIsConnected() const {
	return isConnected;
}

void ProtocolClient::disconnectFromServer() {
	if (getIsConnected()) {
		LOGGER_DEBUG("Closing socket.");
		socket->close();
	}
}

void ProtocolClient::setup() {
	if (!isSetupDone) {
		if (socket == nullptr) {
			socket = new QTcpSocket();
			if (socket == nullptr) {
				return;
			}
		}

		OPENMITTSU_CONNECT(socket, readyRead(), this, socketOnReadyRead());
		OPENMITTSU_CONNECT(socket, error(QAbstractSocket::SocketError), this, socketOnError(QAbstractSocket::SocketError));
		OPENMITTSU_CONNECT(socket, connected(), this, socketConnected());
		OPENMITTSU_CONNECT(socket, disconnected(), this, socketDisconnected());

		outgoingMessagesTimer = new QTimer(this);
		outgoingMessagesTimer->setInterval(500);
		OPENMITTSU_CONNECT(outgoingMessagesTimer, timeout(), this, outgoingMessagesTimerOnTimer());

		acknowledgmentWaitingTimer = new QTimer(this);
		acknowledgmentWaitingTimer->setInterval(5000);
		OPENMITTSU_CONNECT(acknowledgmentWaitingTimer, timeout(), this, acknowledgmentWaitingTimerOnTimer());
		acknowledgmentWaitingTimer->start();

		keepAliveTimer = new QTimer(this);
		keepAliveTimer->setInterval(3 * 60 * 1000);
		keepAliveCounter = 1;
		OPENMITTSU_CONNECT(keepAliveTimer, timeout(), this, keepAliveTimerOnTimer());

		QNetworkConfigurationManager manager;
		if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
			LOGGER_DEBUG("Using Network Manager and Session.");
			// Get saved network configuration
			QSettings settings(QSettings::UserScope, QLatin1String("openMittsu"));
			settings.beginGroup(QLatin1String("QtNetwork"));
			const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
			settings.endGroup();

			// If the saved network configuration is not currently discovered use the system default
			QNetworkConfiguration config = manager.configurationFromIdentifier(id);
			if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
				config = manager.defaultConfiguration();
			}

			networkSession = new QNetworkSession(config, this);
			OPENMITTSU_CONNECT(networkSession, opened(), this, networkSessionOnIsOpen());

			isNetworkSessionReady = false;
			networkSession->open();
		} else {
			LOGGER_DEBUG("Not using the Network Manager and Session.");
			isNetworkSessionReady = true;

			emit readyConnect();
		}

		isSetupDone = true;
		emit setupDone();
	}
}

void ProtocolClient::teardown() {
	if (isSetupDone) {
		OPENMITTSU_DISCONNECT(socket, readyRead(), this, socketOnReadyRead());
		OPENMITTSU_DISCONNECT(socket, error(QAbstractSocket::SocketError), this, socketOnError(QAbstractSocket::SocketError));
		OPENMITTSU_DISCONNECT(socket, connected(), this, socketConnected());
		OPENMITTSU_DISCONNECT(socket, disconnected(), this, socketDisconnected());

		OPENMITTSU_DISCONNECT(outgoingMessagesTimer, timeout(), this, outgoingMessagesTimerOnTimer());
		OPENMITTSU_DISCONNECT(acknowledgmentWaitingTimer, timeout(), this, acknowledgmentWaitingTimerOnTimer());
		OPENMITTSU_DISCONNECT(keepAliveTimer, timeout(), this, keepAliveTimerOnTimer());

		if (isConnected) {
			LOGGER()->info("Disconnecting from Server on teardown.");
			socket->close();
			isConnected = false;
		}

		socket->deleteLater();
		socket = nullptr;

		isNetworkSessionReady = false;
		if (networkSession != nullptr) {
			OPENMITTSU_DISCONNECT(networkSession, opened(), this, networkSessionOnIsOpen());
			networkSession->deleteLater();
			networkSession = nullptr;
		}

		outgoingMessagesTimer->stop();
		outgoingMessagesTimer->deleteLater();
		outgoingMessagesTimer = nullptr;
		
		acknowledgmentWaitingTimer->stop();
		acknowledgmentWaitingTimer->deleteLater();
		acknowledgmentWaitingTimer = nullptr;
		
		keepAliveTimer->stop();
		keepAliveTimer->deleteLater();
		keepAliveTimer = nullptr;

		isSetupDone = false;
	}
	emit teardownComplete();
}

void ProtocolClient::outgoingMessagesTimerOnTimer() {
	outgoingMessagesMutex.lock();
	if (!isConnected || !isAllowedToSend) {
		if (outgoingMessages.size() == 0) {
			outgoingMessagesTimer->stop();
		} else {
			outgoingMessagesTimer->setInterval(500);
			outgoingMessagesTimer->start();
		}
	} else {
		QList<QByteArray>::const_iterator i;
		size_t count = 0;
		for (i = outgoingMessages.constBegin(); i != outgoingMessages.constEnd(); ++i) {
			QByteArray const& data = *i;
			quint16 const size = Endian::uint16FromHostEndianToLittleEndian(static_cast<quint16>(data.size()));
			QByteArray const lengthBytes = ByteArrayConversions::convertQuint16toQByteArray(size);
			// First write the two bytes giving the length of the packet
			socket->write(lengthBytes);
			// Then write the data itself
			socket->write(data);
			socket->flush();

			// Update stats
			bytesSend += (2 + data.size());
			messagesSend += 1;

			++count;
		}
		LOGGER_DEBUG("Wrote {} messages to server.", count);
		outgoingMessages.clear();
		outgoingMessagesTimer->stop();
	}
	outgoingMessagesMutex.unlock();
}

void ProtocolClient::acknowledgmentWaitingTimerOnTimer() {
	if (acknowledgmentWaitingMessages.size() > 0) {
		QDateTime const now = QDateTime::currentDateTime();
		acknowledgmentWaitingMutex.lock();
		QHash<MessageId, std::shared_ptr<AcknowledgmentProcessor>>::iterator i = acknowledgmentWaitingMessages.begin();
		QHash<MessageId, std::shared_ptr<AcknowledgmentProcessor>>::iterator end = acknowledgmentWaitingMessages.end();
		while (i != end) {
			if (i.value()->getTimeoutTime() <= now) {
				i.value()->sendFailed(this, messageCenter, i.key());

				i = acknowledgmentWaitingMessages.erase(i);
			} else {
				++i;
			}
		}
		acknowledgmentWaitingMutex.unlock();
	}
}

void ProtocolClient::keepAliveTimerOnTimer() {
	if (!isConnected || !isAllowedToSend) {
		keepAliveTimer->stop();
	} else {
		++keepAliveCounter;
		// Send Packet.
		LOGGER_DEBUG("Sending Keep-Alive packet with value {}.", keepAliveCounter);
		uint32_t const keepAliveValue = Endian::uint32FromHostEndianToLittleEndian(keepAliveCounter);

		// Signature Byte: 0x00
		QByteArray packet(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, 0x00);
		packet[0] = (PROTO_PACKET_SIGNATURE_KEEPALIVE_REQUEST);

		packet.append(ByteArrayConversions::convertQuint32toQByteArray(keepAliveValue));
		encryptAndSendDataPacketToServer(packet);
	}
}

void ProtocolClient::messageSendFailed(GroupId const& groupId, MessageId const& messageId) {
	QMetaObject::invokeMethod(messageCenter, "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(GroupId, groupId), Q_ARG(MessageId, messageId));
}

void ProtocolClient::messageSendFailed(ContactId const& contactId, MessageId const& messageId) {
	QMetaObject::invokeMethod(messageCenter, "onMessageSendFailed", Qt::QueuedConnection, Q_ARG(ContactId, contactId), Q_ARG(MessageId, messageId));
}

void ProtocolClient::messageSendDone(GroupId const& groupId, MessageId const& messageId) {
	QMetaObject::invokeMethod(messageCenter, "onMessageSendDone", Qt::QueuedConnection, Q_ARG(GroupId, groupId), Q_ARG(MessageId, messageId));
}

void ProtocolClient::messageSendDone(ContactId const& contactId, MessageId const& messageId) {
	QMetaObject::invokeMethod(messageCenter, "onMessageSendDone", Qt::QueuedConnection, Q_ARG(ContactId, contactId), Q_ARG(MessageId, messageId));
}

void ProtocolClient::socketOnReadyRead() {
	if (!isConnected) {
		// ignore until the handshake is complete.
		return;
	}
	QByteArray newData(socket->readAll());
	bytesReceived += newData.size();

	readyReadRemainingData.append(newData);
	int bytesAvailable = readyReadRemainingData.size();

	if (bytesAvailable < PROTO_DATA_HEADER_SIZE_LENGTH_BYTES) {
		// Not enough data, not even the length Bytes!
		return;
	}

	// Unsigned Two-Byte Integer in Little-Endian
	quint16 packetLength = Endian::uint16FromLittleEndianToHostEndian(ByteArrayConversions::convert2ByteQByteArrayToQuint16(readyReadRemainingData.left(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES)));
	if (bytesAvailable < static_cast<int>((PROTO_DATA_HEADER_SIZE_LENGTH_BYTES + packetLength))) {
		// packet not yet complete
		LOGGER_DEBUG("Deferring, packet not yet complete.");
		return;
	}

	// Remove the two length-bytes and packet
	QByteArray const packet = readyReadRemainingData.mid(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES, packetLength);
	readyReadRemainingData = readyReadRemainingData.mid(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES + packetLength, -1);

	QByteArray const decodedPacket = cryptoBox.decryptFromServer(packet);

	// Update stats
	messagesReceived += 1;

	// Handle packet
	// Extract LSB:
	char const packetTypeByte = decodedPacket.at(0);
	QByteArray const packetContents = decodedPacket.mid(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, -1);

	if (packetTypeByte == (PROTO_PACKET_SIGNATURE_SENDING_MSG)) {
		LOGGER()->warn("Received a SENDING packet.\nThis should _NOT_ happen?!\nPayload: {}", QString(decodedPacket.toHex()).toStdString());
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_DELIVERING_MSG)) {
		LOGGER_DEBUG("Received a DELIVERING packet.");
		MessageWithEncryptedPayload const messageWithEncryptedPayload(MessageWithEncryptedPayload::fromPacket(decodedPacket));
		sendClientAcknowlegmentForMessage(messageWithEncryptedPayload);

		handleIncomingMessage(messageWithEncryptedPayload);
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_KEEPALIVE_ANSWER)) {
		LOGGER_DEBUG("Received a KEEP_ALIVE_REPLY packet.");
		handleIncomingKeepAliveAnswer(packetContents);
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_KEEPALIVE_REQUEST)) {
		LOGGER_DEBUG("Received a KEEP_ALIVE_REQUEST packet.");
		handleIncomingKeepAliveRequest(packetContents);
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_SERVER_ACK)) {
		ContactId const senderIdentity(packetContents.left(PROTO_IDENTITY_LENGTH_BYTES));
		MessageId const messageId(packetContents.right(PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES));
		LOGGER_DEBUG("Received a SERVER_ACKNOWLEDGE packet for Message #{}.", messageId.toString());

		handleIncomingAcknowledgment(senderIdentity, messageId);
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CLIENT_ACK)) {
		LOGGER()->warn("Received a CLIENT_ACKNOWLEDGE packet: {}", QString(packetContents.toHex()).toStdString());
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CONNECTION_ESTABLISHED)) {
		LOGGER()->info("Received a CONNECTION_ESTABLISHED packet.");
		isAllowedToSend = true;
		keepAliveCounter = 1;
		keepAliveTimer->start();
	} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CONNECTION_DUPLICATE)) {
		LOGGER()->warn("Received a CONNECTION_DUPLICATE warning, we will be forcefully disconnected after this. Message from Server: {}", QString::fromUtf8(packetContents).toStdString());

		emit duplicateIdUsageDetected();
	} else {
		LOGGER()->warn("Received an UNKNOWN packet with signature {} and payload {}.", QString(decodedPacket.left(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES).toHex()).toStdString(), QString(decodedPacket.toHex()).toStdString());
	}

	// Check if there is more to do
	if (readyReadRemainingData.size() > 0) {
		QTimer::singleShot(0, this, SLOT(socketOnReadyRead()));
	}
}

void ProtocolClient::handleIncomingAcknowledgment(ContactId const& sender, MessageId const& messageId) {
	acknowledgmentWaitingMutex.lock();
	if (acknowledgmentWaitingMessages.contains(messageId)) {
		QHash<MessageId, std::shared_ptr<AcknowledgmentProcessor>>::const_iterator i = acknowledgmentWaitingMessages.constFind(messageId);
		i.value()->sendSuccess(this, messageCenter, messageId);

		acknowledgmentWaitingMessages.remove(messageId);
	}
	acknowledgmentWaitingMutex.unlock();
}

void ProtocolClient::handleIncomingMessage(MessageWithEncryptedPayload const& message) {
	ContactId const& receiver = message.getMessageHeader().getReceiver();
	ContactId const& sender = message.getMessageHeader().getSender();

	if (!(receiver == clientConfiguration.getClientIdentity())) {
		LOGGER()->critical("Received an incoming text message packet, but we are not the receiver.\nIt was intended for {} from sender {}.", receiver.toString(), sender.toString());
	} else {
		if (missingIdentityProcessors.contains(sender)) {
			LOGGER_DEBUG("Enqueing new message on existing MissingIdentityProcessor for ID {}.", sender);
			missingIdentityProcessors.constFind(sender).value()->enqueueMessage(message);
			return;
		} else if ((!cryptoBox.getKeyRegistry().hasIdentity(sender))) {
			std::shared_ptr<MissingIdentityProcessor> missingIdentityProcessor = std::make_shared<MissingIdentityProcessor>(sender);
			missingIdentityProcessor->enqueueMessage(message);
			missingIdentityProcessors.insert(sender, missingIdentityProcessor);
			LOGGER_DEBUG("Enqueing MissingIdentityProcessor for ID {}.", sender.toString());

			CallbackTask* callbackTask = new IdentityReceiverCallbackTask(&serverConfiguration, sender);

			enqeueCallbackTask(callbackTask);
			return;
		}
		
		MessageWithPayload messageWithPayload(message.decrypt(&cryptoBox));
		handleIncomingMessage(messageWithPayload, &message);
	}
}

void ProtocolClient::handleIncomingMessage(MessageWithPayload const& messageWithPayload, MessageWithEncryptedPayload const*const message) {
	try {
		std::shared_ptr<Message> messageSharedPtr = IncomingMessagesParser::parseMessageWithPayloadToMessage(messageWithPayload);
		Message const* messagePtr = messageSharedPtr.get();
		handleIncomingMessage(messagePtr, message);
	} catch (ProtocolErrorException& pee) {
		LOGGER()->warn("Encountered an error while parsing payload of received message from {} with ID {}: {}", messageWithPayload.getMessageHeader().getSender().toString(), messageWithPayload.getMessageHeader().getMessageId().toString(), pee.what());
	} catch (std::exception& e) {
		LOGGER()->critical("Unknown error while parsing payload of received message from {} with ID {}: {}", messageWithPayload.getMessageHeader().getSender().toString(), messageWithPayload.getMessageHeader().getMessageId().toString(), e.what());
	}
}

void ProtocolClient::handleIncomingMessage(Message const*const message, MessageWithEncryptedPayload const*const messageWithEncryptedPayload) {
	if (dynamic_cast<ContactMessage const*>(message) != nullptr) {
		ContactMessage const* contactMessage = dynamic_cast<ContactMessage const*>(message);
		ContactMessageContent const* cmc = contactMessage->getContactMessageContent();
		if (cmc->hasPostReceiveCallbackTask()) {
			CallbackTask* callbackTask = cmc->getPostReceiveCallbackTask(new ContactMessage(*contactMessage), &serverConfiguration, &cryptoBox);

			enqeueCallbackTask(callbackTask);
			return;
		}

		if (dynamic_cast<ContactTextMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<ContactTextMessageContent const>(dynamic_cast<ContactTextMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<ContactImageMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<ContactImageMessageContent const>(dynamic_cast<ContactImageMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<ContactLocationMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<ContactLocationMessageContent const>(dynamic_cast<ContactLocationMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<ReceiptMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<ReceiptMessageContent const>(dynamic_cast<ReceiptMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<UserTypingMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<UserTypingMessageContent const>(dynamic_cast<UserTypingMessageContent const*>(cmc->clone())));
		} else {
			LOGGER()->critical("No match for content of message from {}.", contactMessage->getMessageHeader().getSender().toString());
		}
	} else if (dynamic_cast<SpecializedGroupMessage const*>(message) != nullptr) {
		SpecializedGroupMessage const* groupMessage = dynamic_cast<SpecializedGroupMessage const*>(message);
		GroupMessageContent const* cmc = groupMessage->getGroupMessageContent();

		if (groupsWithMissingIdentities.contains(cmc->getGroupId())) {
			if (messageWithEncryptedPayload == nullptr) {
				LOGGER()->warn("Trying to enqueue new message for group {} on existing MissingIdentityProcessor, but the encryptedPayload pointer is null.", cmc->getGroupId().toString());
				return;
			}

			LOGGER_DEBUG("Enqueuing new message for group {} on existing MissingIdentityProcessor.", cmc->getGroupId().toString());
			groupsWithMissingIdentities.constFind(cmc->getGroupId()).value()->enqueueMessage(*messageWithEncryptedPayload);
			return;
		}

		if (cmc->hasPostReceiveCallbackTask()) {
			CallbackTask* callbackTask = cmc->getPostReceiveCallbackTask(new SpecializedGroupMessage(*groupMessage), &serverConfiguration, &cryptoBox);

			enqeueCallbackTask(callbackTask);
			return;
		}

		if (dynamic_cast<GroupTextMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupTextMessageContent const>(dynamic_cast<GroupTextMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupImageMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupImageMessageContent const>(dynamic_cast<GroupImageMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupLocationMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupLocationMessageContent const>(dynamic_cast<GroupLocationMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupFileMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupFileMessageContent const>(dynamic_cast<GroupFileMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupCreationMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupCreationMessageContent const>(dynamic_cast<GroupCreationMessageContent const*>(cmc->clone())), messageWithEncryptedPayload);
		} else if (dynamic_cast<GroupSetTitleMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupSetTitleMessageContent const>(dynamic_cast<GroupSetTitleMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupSetPhotoMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupSetPhotoMessageContent const>(dynamic_cast<GroupSetPhotoMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupSyncMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupSyncMessageContent const>(dynamic_cast<GroupSyncMessageContent const*>(cmc->clone())));
		} else if (dynamic_cast<GroupLeaveMessageContent const*>(cmc) != nullptr) {
			handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<GroupLeaveMessageContent const>(dynamic_cast<GroupLeaveMessageContent const*>(cmc->clone())));
		} else {
			LOGGER()->critical("No match for content of group message from {}.", groupMessage->getMessageHeader().getSender().toString());
		}
	} else {
		LOGGER()->critical("Not a contact message, failed to parse message from {}.", message->getMessageHeader().getSender().toString());
	}
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactTextMessageContent const> contactTextMessageContent) {
	LOGGER_DEBUG("Received a message from {} with {} Bytes of text.", messageHeader.getSender().toString(), contactTextMessageContent->getText().size());
	QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageText", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(QString, contactTextMessageContent->getText()));
}

bool ProtocolClient::validateGroupMembershipAndInfo(GroupId const& groupId, ContactId const& sender) {
	if (!groupRegistry.hasGroup(groupId)) {
		sendGroupSyncRequest(groupId);
		return false;
	} else if (!groupRegistry.getGroupMembers(groupId).contains(sender)) {
		sendGroupSyncRequest(groupId);
		return false;
	}

	return true;
}

void ProtocolClient::sendGroupSyncRequest(GroupId const& groupId) {
	if (groupId.getOwner() == clientConfiguration.getClientIdentity()) {
		LOGGER()->warn("Can not request group data for group {} from self!", groupId.toString());
	} else {
		LOGGER()->info("Requesting group sync from owner of group {}.", groupId.toString());
		
		SpecializedGroupMessage groupSyncMessage(FullMessageHeader(groupId.getOwner(), MessageTime::now(), clientConfiguration.getClientIdentity(), uniqueMessageIdGenerator->getNextUniqueMessageId(groupId), MessageFlagsFactory::createGroupControlMessageFlags(), *pushFromIdPtr), new GroupSyncMessageContent(groupId));
		std::shared_ptr<AcknowledgmentProcessor> ap = std::make_shared<GroupCreationMessageAcknowledgmentProcessor>(groupId, QDateTime::currentDateTime().addSecs(15), groupSyncMessage.getMessageHeader().getMessageId(), false);
		handleOutgoingMessage(&groupSyncMessage, ap);
	}
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupTextMessageContent const> groupTextMessageContent) {
	if (!validateGroupMembershipAndInfo(groupTextMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupTextMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a group message from {} to group {} with {} Bytes of text.", messageHeader.getSender().toString(), groupTextMessageContent->getGroupId().toString(), groupTextMessageContent->getText().size());
	QMetaObject::invokeMethod(messageCenter, "onReceivedGroupContactMessageText", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(GroupId, groupTextMessageContent->getGroupId()), Q_ARG(QString, groupTextMessageContent->getText()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactImageMessageContent const> contactImageMessageContent) {
	LOGGER_DEBUG("Received an image message from {}.", messageHeader.getSender().toString());
	QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageImage", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(QByteArray, contactImageMessageContent->getImageData()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupImageMessageContent const> groupImageMessageContent) {
	if (!validateGroupMembershipAndInfo(groupImageMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupImageMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received an image message from {} to group {}.", messageHeader.getSender().toString(), groupImageMessageContent->getGroupId().toString());
	QMetaObject::invokeMethod(messageCenter, "onReceivedGroupContactMessageImage", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(GroupId, groupImageMessageContent->getGroupId()), Q_ARG(QByteArray, groupImageMessageContent->getImageData()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ContactLocationMessageContent const> contactLocationMessageContent) {
	LOGGER_DEBUG("Received a location message from {}.", messageHeader.getSender().toString());
	QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageLocation", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(double, contactLocationMessageContent->getLatitude()), Q_ARG(double, contactLocationMessageContent->getLongitude()), Q_ARG(double, contactLocationMessageContent->getHeight()), Q_ARG(QString, contactLocationMessageContent->getDescription()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupLocationMessageContent const> groupLocationMessageContent) {
	if (!validateGroupMembershipAndInfo(groupLocationMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupLocationMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a location message from {} to group {}.", messageHeader.getSender().toString(), groupLocationMessageContent->getGroupId().toString());
	QMetaObject::invokeMethod(messageCenter, "onReceivedGroupContactMessageLocation", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(GroupId, groupLocationMessageContent->getGroupId()), Q_ARG(double, groupLocationMessageContent->getLatitude()), Q_ARG(double, groupLocationMessageContent->getLongitude()), Q_ARG(double, groupLocationMessageContent->getHeight()), Q_ARG(QString, groupLocationMessageContent->getDescription()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupFileMessageContent const> groupFileMessageContent) {
	if (!validateGroupMembershipAndInfo(groupFileMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupFileMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a file message from {} to group {}. THIS IS STILL UNSUPPORTED.", messageHeader.getSender().toString(), groupFileMessageContent->getGroupId().toString());
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupCreationMessageContent const> groupCreationMessageContent, MessageWithEncryptedPayload const*const messageWithEncryptedPayload) {
	QSet<ContactId> const groupMembers = groupCreationMessageContent->getGroupMembers();
	QSet<ContactId>::const_iterator it = groupMembers.constBegin();
	QSet<ContactId>::const_iterator end = groupMembers.constEnd();

	QSet<ContactId> missingIds;
	for (; it != end; ++it) {
		if (missingIdentityProcessors.contains(*it)) {
			if (messageWithEncryptedPayload == nullptr) {
				LOGGER()->warn("Trying to handle GroupCreationMessage for group {} on existing MissingIdentityProcessor, but the encryptedPayload pointer is null.", groupCreationMessageContent->getGroupId().toString());
				return;
			}

			LOGGER_DEBUG("Enqueing group creation message into existing MissingIdentityProcessor for ID {}.", it->toString());
			missingIdentityProcessors.constFind(*it).value()->enqueueMessage(*messageWithEncryptedPayload);
			return;
		} else if (!cryptoBox.getKeyRegistry().hasIdentity(*it)) {
			missingIds.insert(*it);
		}
	}
	if (!missingIds.isEmpty()) {
		if (messageWithEncryptedPayload == nullptr) {
			LOGGER()->warn("Trying to handle GroupCreationMessage for group {} with new MissingIdentityProcessor, but the encryptedPayload pointer is null.", groupCreationMessageContent->getGroupId().toString());
			return;
		}

		std::shared_ptr<MissingIdentityProcessor> missingIdentityProcessor = std::make_shared<MissingIdentityProcessor>(groupCreationMessageContent->getGroupId(), missingIds);
		missingIdentityProcessor->enqueueMessage(*messageWithEncryptedPayload);
		groupsWithMissingIdentities.insert(groupCreationMessageContent->getGroupId(), missingIdentityProcessor);
		QSet<ContactId>::const_iterator itMissing = missingIds.constBegin();
		QSet<ContactId>::const_iterator endMissing = missingIds.constEnd();
		for (; itMissing != endMissing; ++itMissing) {
			LOGGER_DEBUG("Enqueing MissingIdentityProcessor for group member with ID {}.", itMissing->toString());
			missingIdentityProcessors.insert(*itMissing, missingIdentityProcessor);

			CallbackTask* callbackTask = new IdentityReceiverCallbackTask(&serverConfiguration, *itMissing);
			enqeueCallbackTask(callbackTask);
		}
		return;
	}

	if (groupRegistry.hasGroup(groupCreationMessageContent->getGroupId())) {
		groupRegistry.updateGroupMembers(groupCreationMessageContent->getGroupId(), groupCreationMessageContent->getGroupMembers());
		LOGGER_DEBUG("Received a group update message for group {}, group now has {} instead of {} members.", groupCreationMessageContent->getGroupId().toString(), groupCreationMessageContent->getGroupMembers().size(), groupRegistry.getGroupMembers(groupCreationMessageContent->getGroupId()).size());
		QMetaObject::invokeMethod(messageCenter, "onGroupSync", Qt::QueuedConnection, Q_ARG(GroupId, groupCreationMessageContent->getGroupId()), Q_ARG(QSet<ContactId>, groupCreationMessageContent->getGroupMembers()));
	} else {
		groupRegistry.addGroup(groupCreationMessageContent->getGroupId(), groupCreationMessageContent->getGroupMembers(), "");
		LOGGER_DEBUG("Received a group creation message for group {} with {} members.", groupCreationMessageContent->getGroupId().toString(), groupCreationMessageContent->getGroupMembers().size());
		QMetaObject::invokeMethod(messageCenter, "onFoundNewGroup", Qt::QueuedConnection, Q_ARG(GroupId, groupCreationMessageContent->getGroupId()), Q_ARG(QSet<ContactId>, groupCreationMessageContent->getGroupMembers()));
	}
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSetPhotoMessageContent const> groupSetPhotoMessageContent) {
	if (!validateGroupMembershipAndInfo(groupSetPhotoMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupSetPhotoMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a group SetPhoto message from {} to group {}.", messageHeader.getSender().toString(), groupSetPhotoMessageContent->getGroupId().toString());
	QMetaObject::invokeMethod(messageCenter, "onGroupSetImage", Qt::QueuedConnection, Q_ARG(GroupId, groupSetPhotoMessageContent->getGroupId()), Q_ARG(QByteArray, groupSetPhotoMessageContent->getGroupPhoto()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSetTitleMessageContent const> groupSetTitleMessageContent) {
	if (!validateGroupMembershipAndInfo(groupSetTitleMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupSetTitleMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a group SetTitle message from {} to group {}.", messageHeader.getSender().toString(), groupSetTitleMessageContent->getGroupId().toString());
	QMetaObject::invokeMethod(messageCenter, "onGroupSetTitle", Qt::QueuedConnection, Q_ARG(GroupId, groupSetTitleMessageContent->getGroupId()), Q_ARG(QString, groupSetTitleMessageContent->getTitle()));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupSyncMessageContent const> groupSyncMessageContent) {
	if (!groupRegistry.hasGroup(groupSyncMessageContent->getGroupId())) {
		LOGGER()->warn("Received a group SyncRequest message from {} to group {}, but we do not know about that group!", messageHeader.getSender().toString(), groupSyncMessageContent->getGroupId().toString());
		return;
	}

	SpecializedGroupMessage groupCreationMessage(FullMessageHeader(messageHeader.getSender(), MessageTime::now(), clientConfiguration.getClientIdentity(), uniqueMessageIdGenerator->getNextUniqueMessageId(groupSyncMessageContent->getGroupId()), MessageFlagsFactory::createGroupControlMessageFlags(), *pushFromIdPtr), new GroupCreationMessageContent(groupSyncMessageContent->getGroupId(), groupRegistry.getGroupMembers(groupSyncMessageContent->getGroupId())));
	std::shared_ptr<AcknowledgmentProcessor> apCreate = std::make_shared<GroupCreationMessageAcknowledgmentProcessor>(groupSyncMessageContent->getGroupId(), QDateTime::currentDateTime().addSecs(15), groupCreationMessage.getMessageHeader().getMessageId(), false);
	handleOutgoingMessage(&groupCreationMessage, apCreate);

	SpecializedGroupMessage groupTitleMessage(FullMessageHeader(messageHeader.getSender(), MessageTime::now(), clientConfiguration.getClientIdentity(), uniqueMessageIdGenerator->getNextUniqueMessageId(groupSyncMessageContent->getGroupId()), MessageFlagsFactory::createGroupControlMessageFlags(), *pushFromIdPtr), new GroupSetTitleMessageContent(groupSyncMessageContent->getGroupId(), groupRegistry.getGroupTitle(groupSyncMessageContent->getGroupId())));
	std::shared_ptr<AcknowledgmentProcessor> apTitle = std::make_shared<GroupCreationMessageAcknowledgmentProcessor>(groupSyncMessageContent->getGroupId(), QDateTime::currentDateTime().addSecs(15), groupTitleMessage.getMessageHeader().getMessageId(), false);
	handleOutgoingMessage(&groupTitleMessage, apTitle);
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<GroupLeaveMessageContent const> groupLeaveMessageContent) {
	if (!validateGroupMembershipAndInfo(groupLeaveMessageContent->getGroupId(), messageHeader.getSender())) {
		LOGGER()->warn("Ignoring message from {} to Group {} since group does not exist or does not contain the Sender.", messageHeader.getSender().toString(), groupLeaveMessageContent->getGroupId().toString());
		return;
	}

	LOGGER_DEBUG("Received a group leave message from {} to group {}.", messageHeader.getSender().toString(), groupLeaveMessageContent->getGroupId().toString());
	groupRegistry.removeMember(groupLeaveMessageContent->getGroupId(), groupLeaveMessageContent->getLeavingContactId());
	QMetaObject::invokeMethod(messageCenter, "onGroupSync", Qt::QueuedConnection, Q_ARG(GroupId, groupLeaveMessageContent->getGroupId()), Q_ARG(QSet<ContactId>, groupRegistry.getGroupMembers(groupLeaveMessageContent->getGroupId())));
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<ReceiptMessageContent const> receiptMessageContent) {
	ReceiptMessageContent::ReceiptType receiptType = receiptMessageContent->getReceiptType();
	MessageId const refMessageId = receiptMessageContent->getReferredMessageId();

	switch (receiptType) {
	case ReceiptMessageContent::ReceiptType::RECEIVED:
		LOGGER_DEBUG("User {} has received message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageReceiptReceived", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(MessageId, refMessageId));
		break;
	case ReceiptMessageContent::ReceiptType::SEEN:
		LOGGER_DEBUG("User {} has seen message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageReceiptSeen", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(MessageId, refMessageId));
		break;
	case ReceiptMessageContent::ReceiptType::AGREE:
		LOGGER_DEBUG("User {} has agreed with message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageReceiptAgree", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(MessageId, refMessageId));
		break;
	case ReceiptMessageContent::ReceiptType::DISAGREE:
		LOGGER_DEBUG("User {} has disagreed with message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactMessageReceiptDisagree", Qt::QueuedConnection, Q_ARG(FullMessageHeader, messageHeader), Q_ARG(MessageId, refMessageId));
		break;
	default:
		throw InternalErrorException() << "Unknown ReceiptType in handleIncomingMessage(receiptMessageContent)!";
		break;
	}
}

void ProtocolClient::handleIncomingMessage(FullMessageHeader const& messageHeader, std::shared_ptr<UserTypingMessageContent const> userTypingMessageContent) {
	if (userTypingMessageContent->isUserTyping()) {
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactStartedTypingNotification", Qt::QueuedConnection, Q_ARG(ContactId, messageHeader.getSender()));
	} else {
		QMetaObject::invokeMethod(messageCenter, "onReceivedIdentityContactStoppedTypingNotification", Qt::QueuedConnection, Q_ARG(ContactId, messageHeader.getSender()));
	}
}

void ProtocolClient::handleIncomingKeepAliveRequest(QByteArray const& packetData) {
	if (packetData.size() > (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES)) {
		throw ProtocolErrorException() << "Invalid KeepAliveRequest packet from the server, size mismatch (" << packetData.size() << " > " << (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES) << " Bytes).";
	}

	LOGGER_DEBUG("Sending reply to Server KEEP_ALIVE_REQUEST with {} Bytes.", packetData.size());
	QByteArray packet(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, 0x00);
	packet[0] = (PROTO_PACKET_SIGNATURE_KEEPALIVE_ANSWER);

	packet.append(packetData);
	encryptAndSendDataPacketToServer(packet);
}

void ProtocolClient::handleIncomingKeepAliveAnswer(QByteArray const& packetData) {
	if (packetData.size() > (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES)) {
		throw ProtocolErrorException() << "Invalid KeepAlive packet from the server, size mismatch (" << packetData.size() << " > " << (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES) << " Bytes).";
	}
	
	if (packetData.size() == sizeof(keepAliveCounter)) {
		quint32 const keepAliveCounterReplyValue = Endian::uint32FromLittleEndianToHostEndian(ByteArrayConversions::convert4ByteQByteArrayToQuint32(packetData));
		if (keepAliveCounterReplyValue != keepAliveCounter) {
			LOGGER()->warn("The keep-alive reply is not identical to what we send out. It should be {}, but it is {}.", keepAliveCounter, keepAliveCounterReplyValue);
		}
	} else {
		LOGGER()->warn("Received a keep alive reply with a size that does not match the size we send out ({} Bytes vs. {} Bytes).", packetData.size(), sizeof(keepAliveCounter));
	}
}

void ProtocolClient::sendContactMessage(PreliminaryContactMessage const& preliminaryMessage) {
	if (preliminaryMessage.getPreliminaryMessageHeader()->getReceiver() == clientConfiguration.getClientIdentity()) {
		return;
	}
	
	ContactMessage contactMessage(FullMessageHeader(preliminaryMessage.getPreliminaryMessageHeader(), clientConfiguration.getClientIdentity(), *pushFromIdPtr), preliminaryMessage.getPreliminaryMessageContent()->clone());
	std::shared_ptr<AcknowledgmentProcessor> ap = std::make_shared<ContactMessageAcknowledgmentProcessor>(contactMessage.getMessageHeader().getReceiver(), QDateTime::currentDateTime().addSecs(15), contactMessage.getMessageHeader().getMessageId());
	handleOutgoingMessage(&contactMessage, ap);
}

void ProtocolClient::handleOutgoingMessage(ContactMessage const*const contactMessage, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor) {
	if (contactMessage->getContactMessageContent()->hasPreSendCallbackTask()) {
		CallbackTask* callbackTask = contactMessage->getContactMessageContent()->getPreSendCallbackTask(new ContactMessage(*contactMessage), acknowledgmentProcessor, &serverConfiguration, &cryptoBox);

		enqeueCallbackTask(callbackTask);
		return;
	}

	LOGGER_DEBUG("Sending Message to Contact {} with ID {}.", contactMessage->getMessageHeader().getReceiver().toString(), contactMessage->getMessageHeader().getMessageId().toString());

	MessageWithPayload messageWithPayload(contactMessage->getMessageHeader(), contactMessage->getContactMessageContent()->toPacketPayload());
	MessageWithEncryptedPayload messageWithEncryptedPayload(messageWithPayload.encrypt(&cryptoBox));

	encryptAndSendDataPacketToServer(messageWithEncryptedPayload.toPacket());

	if (!contactMessage->getMessageHeader().getFlags().isNoAckExpectedForMessage()) {
		enqeueWaitForAcknowledgment(contactMessage->getMessageHeader().getMessageId(), acknowledgmentProcessor);
	}
}

void ProtocolClient::handleOutgoingMessage(UnspecializedGroupMessage const*const message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor) {
	if (message->getGroupMessageContent()->hasPreSendCallbackTask()) {
		CallbackTask* callbackTask = message->getGroupMessageContent()->getPreSendCallbackTask(new UnspecializedGroupMessage(*message), acknowledgmentProcessor, &serverConfiguration, &cryptoBox);

		enqeueCallbackTask(callbackTask);
		return;
	}

	QSet<ContactId> groupMembers = groupRegistry.getGroupMembers(message->getGroupMessageContent()->getGroupId());

	QSet<ContactId>::const_iterator it = groupMembers.constBegin();
	QSet<ContactId>::const_iterator end = groupMembers.constEnd();
	for (; it != end; ++it) {
		if (!(*it == clientConfiguration.getClientIdentity())) {
			SpecializedGroupMessage groupMessageForSingleContact(*message, *it, uniqueMessageIdGenerator->getNextUniqueMessageId(*it));

			handleOutgoingMessage(&groupMessageForSingleContact, acknowledgmentProcessor);
		}
	}
}

void ProtocolClient::handleOutgoingMessage(SpecializedGroupMessage const*const message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor) {
	LOGGER_DEBUG("Sending GroupMessage to Contact {} with ID {}.", message->getMessageHeader().getReceiver().toString(), message->getMessageHeader().getMessageId().toString());

	MessageWithPayload messageWithPayload(message->getMessageHeader(), message->getGroupMessageContent()->toPacketPayload());
	MessageWithEncryptedPayload messageWithEncryptedPayload(messageWithPayload.encrypt(&cryptoBox));

	encryptAndSendDataPacketToServer(messageWithEncryptedPayload.toPacket());

	if (!message->getMessageHeader().getFlags().isNoAckExpectedForMessage()) {
		enqeueWaitForAcknowledgment(message->getMessageHeader().getMessageId(), acknowledgmentProcessor);
	}
}

void ProtocolClient::sendGroupMessage(PreliminaryGroupMessage const& preliminaryMessage) {
	GroupId const& groupId = preliminaryMessage.getPreliminaryMessageHeader()->getGroup();
	if (groupRegistry.hasGroup(groupId)) {
		UnspecializedGroupMessage groupMessage(preliminaryMessage, clientConfiguration.getClientIdentity(), *pushFromIdPtr);
		std::shared_ptr<AcknowledgmentProcessor> ap = std::make_shared<GroupContentMessageAcknowledgmentProcessor>(groupId, QDateTime::currentDateTime().addSecs(15), groupMessage.getMessageHeader().getMessageId());
		handleOutgoingMessage(&groupMessage, ap);
	} else {
		LOGGER()->warn("Ignoring group message for unknown group {}.", groupId.toString());
	}
}

/*
void ProtocolClient::handleIncomingTextMessageTypeGroupCreation(TextMessage const& message) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QByteArray const groupIdBytes(message.getDecryptedMessage().mid(1, PROTO_GROUP_GROUPID_LENGTH_BYTES));
	quint64 const groupId = IdentityHelper::groupIdByteArrayToUint64(groupIdBytes);

	QByteArray const groupMemberIds(message.getDecryptedMessage().mid(9, -1));
	if ((groupMemberIds.size() == 0) || ((groupMemberIds.size() % PROTO_IDENTITY_LENGTH_BYTES) != 0)) {
		throw ProtocolErrorException() << QString("Received illegal group creation message with incorrect size of group member block of %1 Bytes").arg(groupMemberIds.size()).toStdString();
	}
	int position = 0;
	QSet<quint64> newMembers;
	while (position < groupMemberIds.size()) {
		quint64 const memberId = TextMessage::convertIdToUint64(groupMemberIds.mid(position, PROTO_IDENTITY_LENGTH_BYTES));

		if ((!contactRegistry->hasIdentity(memberId)) && (!contactRegistry->addContactFromServer(serverConfiguration, memberId))) {
			throw ProtocolErrorException() << QString("Received an incoming group creation message containing member %1, but we do not have the public key of this member and could not fetch it from a registered identity server.").arg(IdentityHelper::uint64ToIdentityString(memberId)).toStdString();
		}

		newMembers.insert(memberId);
		position += PROTO_IDENTITY_LENGTH_BYTES;
	}

	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		if (!gc->hasMember(message.getSender())) {
			throw ProtocolErrorException() << QString("A non-group member tried to change an existing group!").toStdString();
		} else {
			// Validate that no one was left out
			QSet<quint64>::const_iterator itExisting = gc->getGroupMembers().constBegin();
			QSet<quint64>::const_iterator endExisting = gc->getGroupMembers().constEnd();
			for (; itExisting != endExisting; ++itExisting) {
				if (!newMembers.contains(*itExisting)) {
					throw ProtocolErrorException() << QString("A group-member tried kicking another member out by GROUP_CREATE!").toStdString();
				}
			}

			gc->updateMembers(newMembers);
		}
	} else {
		GroupContact* gc = new GroupContact(groupId, message.getSender());
		gc->updateMembers(newMembers);
		if (!newMembers.contains(message.getSender())) {
			gc->addMember(message.getSender());
		}
		contactRegistry->addContact(gc);
	}
}

void ProtocolClient::handleIncomingTextMessageTypeGroupTitleChange(TextMessage const& message) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QByteArray const groupIdBytes(message.getDecryptedMessage().mid(1, PROTO_GROUP_GROUPID_LENGTH_BYTES));
	quint64 const groupId = IdentityHelper::groupIdByteArrayToUint64(groupIdBytes);

	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		if (gc->getGroupOwner() != message.getSender()) {
			throw ProtocolErrorException() << QString("A non-group owner tried to change the name of an existing group!").toStdString();
		}
		QString const groupName(message.getDecryptedMessage().mid(9, -1));
		gc->setGroupName(groupName);
	} else {
		LOGGER()->warn("Received new title for a group we do not know!");
	}
}

void ProtocolClient::handleIncomingTextMessageTypeGroupMessage(TextMessage const& message) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QByteArray const ownerUserIdBytes(message.getDecryptedMessage().mid(1, PROTO_IDENTITY_LENGTH_BYTES));
	quint64 const ownerUser = IdentityHelper::identityStringToUint64(ownerUserIdBytes);

	QByteArray const groupIdBytes(message.getDecryptedMessage().mid(9, PROTO_GROUP_GROUPID_LENGTH_BYTES));
	quint64 const groupId = IdentityHelper::groupIdByteArrayToUint64(groupIdBytes);

	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		if (!gc->hasMember(message.getSender())) {
			throw ProtocolErrorException() << QString("A non-group member tried to send a message to an existing group!").toStdString();
		} else if (gc->getGroupOwner() != ownerUser) {
			throw ProtocolErrorException() << QString("A group member send a message with a different group owner!").toStdString();
		}

		// Give the Client the opportunity to open a window for the Group.
		emit receivedMessageForGroup(groupId);

		QString const groupMessage(message.getDecryptedMessage().mid(17, -1));
		QMetaObject::invokeMethod(messageCenter, "onReceivedGroupContactMessageText", Qt::QueuedConnection, Q_ARG(quint64, message.getSender()), Q_ARG(quint64, message.getTimeRaw()), Q_ARG(quint64, message.getId()), Q_ARG(QString, groupMessage));
	} else {
		LOGGER()->warn("Received new message for a group we do not know!");
	}
}

void ProtocolClient::handleIncomingTextMessageTypeGroupLeave(TextMessage const& message) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QByteArray const leavingUserIdBytes(message.getDecryptedMessage().mid(1, PROTO_IDENTITY_LENGTH_BYTES));
	quint64 const leavingUser = IdentityHelper::identityStringToUint64(leavingUserIdBytes);

	QByteArray const groupIdBytes(message.getDecryptedMessage().mid(9, PROTO_GROUP_GROUPID_LENGTH_BYTES));
	quint64 const groupId = IdentityHelper::groupIdByteArrayToUint64(groupIdBytes);

	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		gc->removeMember(leavingUser);
	} else {
		LOGGER()->warn("Received leaving user for a group we do not know!");
	}
}

*/

void ProtocolClient::addContact(ContactId const& contactId, PublicKey const& publicKey) {
	LOGGER_DEBUG("Adding Contact {} with Public Key {} to KeyRegistry received from GUI.", contactId.toString(), publicKey.toString().toStdString());
	cryptoBox.getKeyRegistry().addIdentity(contactId, publicKey);
}

void ProtocolClient::newPushFromId(PushFromId const& newPushFromId) {
	LOGGER_DEBUG("Setting new PushFromId {} received from GUI.", newPushFromId.toString());
	pushFromIdPtr = std::unique_ptr<PushFromId>(new PushFromId(newPushFromId));
}

void ProtocolClient::sendGroupSetup(GroupId const& groupId, QSet<ContactId> const& members, QString const& title) {
	LOGGER_DEBUG("GUI requested group setup for group {}.", groupId.toString());

	groupRegistry.addGroup(groupId, members, title);

	UnspecializedGroupMessage groupCreationMessage(PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, uniqueMessageIdGenerator->getNextUniqueMessageId(groupId), MessageFlagsFactory::createGroupControlMessageFlags()), new GroupCreationMessageContent(groupId, members)), clientConfiguration.getClientIdentity(), *pushFromIdPtr);
	std::shared_ptr<AcknowledgmentProcessor> apCreate = std::make_shared<GroupCreationMessageAcknowledgmentProcessor>(groupId, QDateTime::currentDateTime().addSecs(15), groupCreationMessage.getMessageHeader().getMessageId(), true);
	handleOutgoingMessage(&groupCreationMessage, apCreate);

	UnspecializedGroupMessage groupTitleMessage(PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, uniqueMessageIdGenerator->getNextUniqueMessageId(groupId), MessageFlagsFactory::createGroupControlMessageFlags()), new GroupSetTitleMessageContent(groupId, title)), clientConfiguration.getClientIdentity(), *pushFromIdPtr);
	handleOutgoingMessage(&groupTitleMessage, apCreate);

	LOGGER_DEBUG("Adding Group {} with {} members to GroupRegistry received from GUI.", groupId.toString(), members.size());
}

void ProtocolClient::resendGroupSetup(GroupId const& groupId) {
	LOGGER_DEBUG("GUI requested resend of group setup for group {}.", groupId.toString());
	if (!groupRegistry.hasGroup(groupId)) {
		LOGGER()->warn("Ignoring GUI request for resend of group setup for unknown group {}.", groupId.toString());
	} else {
		LOGGER()->info("Resending group setup for group {} with {} members and title \"{}\".", groupId.toString(), groupRegistry.getGroupMembers(groupId).size(), groupRegistry.getGroupTitle(groupId).toStdString());
		UnspecializedGroupMessage groupCreationMessage(PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, uniqueMessageIdGenerator->getNextUniqueMessageId(groupId), MessageFlagsFactory::createGroupControlMessageFlags()), new GroupCreationMessageContent(groupId, groupRegistry.getGroupMembers(groupId))), clientConfiguration.getClientIdentity(), *pushFromIdPtr);
		std::shared_ptr<AcknowledgmentProcessor> apCreate = std::make_shared<GroupCreationMessageAcknowledgmentProcessor>(groupId, QDateTime::currentDateTime().addSecs(15), groupCreationMessage.getMessageHeader().getMessageId(), true);
		handleOutgoingMessage(&groupCreationMessage, apCreate);

		UnspecializedGroupMessage groupTitleMessage(PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, uniqueMessageIdGenerator->getNextUniqueMessageId(groupId), MessageFlagsFactory::createGroupControlMessageFlags()), new GroupSetTitleMessageContent(groupId, groupRegistry.getGroupTitle(groupId))), clientConfiguration.getClientIdentity(), *pushFromIdPtr);
		handleOutgoingMessage(&groupTitleMessage, apCreate);
	}
}

void ProtocolClient::requestGroupSync(GroupId const& groupId) {
	LOGGER_DEBUG("GUI requested a group sync for group {}.", groupId.toString());
	sendGroupSyncRequest(groupId);
}

void ProtocolClient::enqeueCallbackTask(CallbackTask* callbackTask) {
	if (callbackTask == nullptr) {
		LOGGER()->warn("Ignoring nullptr callback task.");
		return;
	}

	OPENMITTSU_CONNECT_QUEUED(callbackTask, finished(CallbackTask*), this, callbackTaskFinished(CallbackTask*));

	QMetaObject::invokeMethod(callbackTask, "start", Qt::QueuedConnection);
}

void ProtocolClient::callbackTaskFinished(CallbackTask* callbackTask) {
	if (dynamic_cast<IdentityReceiverCallbackTask*>(callbackTask) != nullptr) {
		IdentityReceiverCallbackTask* irct = dynamic_cast<IdentityReceiverCallbackTask*>(callbackTask);
		if (!missingIdentityProcessors.contains(irct->getContactIdOfFetchedPublicKey())) {
			LOGGER()->warn("IdentityReceiver CallbackTask finished for ID {}, but no MissingIdentityProcessor is registered for this ID.", irct->getContactIdOfFetchedPublicKey().toString());
		} else {
			std::shared_ptr<MissingIdentityProcessor> missingIdentityProcessor = missingIdentityProcessors.value(irct->getContactIdOfFetchedPublicKey());
			missingIdentityProcessors.remove(irct->getContactIdOfFetchedPublicKey());
			LOGGER_DEBUG("IdentityReceiverCallbackTask finished for ID {}, removing related MissingIdentityProcessor.", irct->getContactIdOfFetchedPublicKey().toString());

			if (!irct->hasFinishedSuccessfully()) {
				LOGGER()->warn("Received a message from user {} that we can not decrypt since the Identity could not be retrieved. Error: {}", irct->getContactIdOfFetchedPublicKey().toString(), irct->getErrorMessage().toStdString());
				missingIdentityProcessor->identityFetcherTaskFinished(irct->getContactIdOfFetchedPublicKey(), false);
			} else {
				missingIdentityProcessor->identityFetcherTaskFinished(irct->getContactIdOfFetchedPublicKey(), true);
				if (cryptoBox.getKeyRegistry().hasIdentity(irct->getContactIdOfFetchedPublicKey())) {
					LOGGER()->warn("Identity {} is already known to KeyRegistry, ignoring result of IdentityReceiverCallbackTask.", irct->getContactIdOfFetchedPublicKey().toString());
				} else {
					cryptoBox.getKeyRegistry().addIdentity(irct->getContactIdOfFetchedPublicKey(), irct->getFetchedPublicKey());

					LOGGER_DEBUG("PublicKey for Identity {} is {}.", irct->getContactIdOfFetchedPublicKey().toString(), irct->getFetchedPublicKey().toString().toStdString());
					QMetaObject::invokeMethod(messageCenter, "onFoundNewContact", Qt::QueuedConnection, Q_ARG(ContactId, irct->getContactIdOfFetchedPublicKey()), Q_ARG(PublicKey, irct->getFetchedPublicKey()));
				}
			}

			if (missingIdentityProcessor->hasFinished()) {
				if (missingIdentityProcessor->hasAssociatedGroupId()) {
					groupsWithMissingIdentities.remove(missingIdentityProcessor->getAssociatedGroupId());
				}
				if (missingIdentityProcessor->hasFinishedSuccessfully()) {
					LOGGER()->info("MissingIdentityProcessor finished successfully, now processing {} queued messages.", missingIdentityProcessor->getQueuedMessages().size());
					std::list<MessageWithEncryptedPayload> queuedMessages(missingIdentityProcessor->getQueuedMessages());
					std::list<MessageWithEncryptedPayload>::const_iterator it = queuedMessages.cbegin();
					std::list<MessageWithEncryptedPayload>::const_iterator end = queuedMessages.cend();
					for (; it != end; ++it) {
						handleIncomingMessage(*it);
					}
				} else {
					LOGGER()->warn("MissingIdentityProcessor failed, will now discard {} messages.", missingIdentityProcessor->getQueuedMessages().size());
				}
			}
		}
		irct->deleteLater();
	} else if (dynamic_cast<MessageCallbackTask*>(callbackTask) != nullptr) {
		MessageCallbackTask* messageCallbackTask = dynamic_cast<MessageCallbackTask*>(callbackTask);
		if (messageCallbackTask->getInitialMessage()->getMessageHeader().getSender() == clientConfiguration.getClientIdentity()) {
			// Sending
			if (!messageCallbackTask->hasFinishedSuccessfully()) {
				LOGGER()->warn("Tried sending a message to user {} that triggered a Callback Task which did not succeed. Error: {}", messageCallbackTask->getInitialMessage()->getMessageHeader().getReceiver().toString(), messageCallbackTask->getErrorMessage().toStdString());
				messageCallbackTask->getAcknowledgmentProcessor()->sendFailed(this, messageCenter, messageCallbackTask->getInitialMessage()->getMessageHeader().getMessageId());
			} else {
				Message* nextMessage = messageCallbackTask->getResultMessage();
				if (dynamic_cast<ContactMessage*>(nextMessage) != nullptr) {
					handleOutgoingMessage(dynamic_cast<ContactMessage*>(nextMessage), messageCallbackTask->getAcknowledgmentProcessor());
				} else if (dynamic_cast<UnspecializedGroupMessage*>(nextMessage) != nullptr) {
					handleOutgoingMessage(dynamic_cast<UnspecializedGroupMessage*>(nextMessage), messageCallbackTask->getAcknowledgmentProcessor());
				} else {
					LOGGER()->critical("MessageCallbackTask finished for an unknown and unhandled Message type.");
				}

				delete nextMessage;
			}
		} else {
			// Receiving
			if (!messageCallbackTask->hasFinishedSuccessfully()) {
				LOGGER()->warn("Received a message from user {} that triggered a Callback Task which did not succeed. Error: {}", messageCallbackTask->getInitialMessage()->getMessageHeader().getSender().toString(), messageCallbackTask->getErrorMessage().toStdString());
			} else {
				Message* nextMessage = messageCallbackTask->getResultMessage();
				handleIncomingMessage(nextMessage, nullptr);

				delete nextMessage;
			}
		}
		
		if (messageCallbackTask->isFinished()) {
			LOGGER_DEBUG("CallbaskTask::isFinished() is true, calling deleteLater() directly.");
			QMetaObject::invokeMethod(messageCallbackTask, "deleteLater", Qt::QueuedConnection);
		} else {
			LOGGER_DEBUG("CallbackTask::isFinished() is false, connection signal.");
			OPENMITTSU_CONNECT_QUEUED(callbackTask, finished(), callbackTask, deleteLater());
		}
	} else {
		LOGGER()->warn("Unhandled callback task of unknown type?!");
		callbackTask->deleteLater();
	}
}

/*
void ProtocolClient::sendGroupMessage(quint64 groupId, QString const& message) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();
	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		quint64 groupOwner = gc->getGroupOwner();
		QSet<quint64>::const_iterator it = gc->getGroupMembers().constBegin();
		QSet<quint64>::const_iterator end = gc->getGroupMembers().constEnd();
		for (; it != end; ++it) {
			if (*it == clientConfiguration.getClientIdentity()) {
				continue;
			}
			TextMessage tm(TextMessage::buildGroupTextMessage(clientConfiguration.getClientIdentity(), *it, groupId, groupOwner, message));
			EncryptedTextMessage etm(tm.encryptMessage(ContactRegistry::getInstance()->getPublicKey(tm.getReceiver()), clientConfiguration.getClientLongTermKeyPair()));

			encryptAndSendDataPacketToServer(etm.toPacket());
		}
	} else {
		throw InternalErrorException() << QString("Can not send a message to unknown group %1").arg(QString(IdentityHelper::uint64ToGroupIdByteArray(groupId).toHex())).toStdString();
	}
}

void ProtocolClient::sendGroupCreation(quint64 groupId, bool isInitialCreation) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();
	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		quint64 groupOwner = gc->getGroupOwner();

		QSet<quint64> members(gc->getGroupMembers());
		if (isInitialCreation) {
			members.remove(groupOwner);
		}

		QSet<quint64>::const_iterator it = members.constBegin();
		QSet<quint64>::const_iterator end = members.constEnd();
		for (; it != end; ++it) {
			if (*it == clientConfiguration.getClientIdentity()) {
				continue;
			}
			TextMessage tm(TextMessage::buildGroupCreationMessage(clientConfiguration.getClientIdentity(), *it, groupId, members));
			EncryptedTextMessage etm(tm.encryptMessage(ContactRegistry::getInstance()->getPublicKey(tm.getReceiver()), clientConfiguration.getClientLongTermKeyPair()));

			encryptAndSendDataPacketToServer(etm.toPacket());
		}
	} else {
		throw InternalErrorException() << QString("Can not send group creation message to unknown group %1").arg(QString(IdentityHelper::uint64ToGroupIdByteArray(groupId).toHex())).toStdString();
	}
}

void ProtocolClient::sendGroupTitle(quint64 groupId, QString const& groupTitle) {
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();
	if (contactRegistry->hasGroup(groupId)) {
		GroupContact* gc = contactRegistry->getGroup(groupId);
		quint64 groupOwner = gc->getGroupOwner();

		QSet<quint64>::const_iterator it = gc->getGroupMembers().constBegin();
		QSet<quint64>::const_iterator end = gc->getGroupMembers().constEnd();
		for (; it != end; ++it) {
			if (*it == clientConfiguration.getClientIdentity()) {
				continue;
			}
			TextMessage tm(TextMessage::buildGroupTitleChangeMessage(clientConfiguration.getClientIdentity(), *it, groupId, groupTitle));
			EncryptedTextMessage etm(tm.encryptMessage(ContactRegistry::getInstance()->getPublicKey(tm.getReceiver()), clientConfiguration.getClientLongTermKeyPair()));

			encryptAndSendDataPacketToServer(etm.toPacket());
		}
	} else {
		throw InternalErrorException() << QString("Can not send group title change message to unknown group %1").arg(QString(IdentityHelper::uint64ToGroupIdByteArray(groupId).toHex())).toStdString();
	}
}

*/

void ProtocolClient::sendClientAcknowlegmentForMessage(MessageWithEncryptedPayload const& message) {
	LOGGER_DEBUG("Sending client acknowledgment to server for message #{}.", message.getMessageHeader().getMessageId().toString());
	encryptAndSendDataPacketToServer(ClientAcknowledgement(message.getMessageHeader().getSender(), message.getMessageHeader().getMessageId()).toPacket());
}

void ProtocolClient::encryptAndSendDataPacketToServer(QByteArray const& dataPacket) {
	outgoingMessagesMutex.lock();
	outgoingMessages.append(cryptoBox.encryptForServer(dataPacket));
	outgoingMessagesTimer->start(0);
	outgoingMessagesMutex.unlock();
}

void ProtocolClient::enqeueWaitForAcknowledgment(MessageId const& messageId, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor) {
	acknowledgmentWaitingMutex.lock();
	
	acknowledgmentProcessor->addMessage(messageId);

	acknowledgmentWaitingMessages.insert(messageId, acknowledgmentProcessor);
	acknowledgmentWaitingMutex.unlock();
}

void ProtocolClient::socketOnError(QAbstractSocket::SocketError socketError) {
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		LOGGER_DEBUG("SocketError: RemoteHostClosedError");
		emit connectToFinished(-1, tr("Remote Host Closed Connection."));
		break;
	case QAbstractSocket::HostNotFoundError:
		LOGGER_DEBUG("SocketError: HostNotFoundError");
		emit connectToFinished(-2, tr("The host was not found. Please check the host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		LOGGER_DEBUG("SocketError: ConnectionRefusedError");
		emit connectToFinished(-3, tr("The connection was refused by the peer. Make sure the server is running, and check that the host name and port settings are correct."));
		break;
	default:
		LOGGER_DEBUG("SocketError: {}", socket->errorString().toStdString());
		emit connectToFinished(-4, tr("The following error occurred: %1.").arg(socket->errorString()));
	}
}

void ProtocolClient::networkSessionOnIsOpen() {
	// Save the used configuration
	QNetworkConfiguration config = networkSession->configuration();
	QString id;
	if (config.type() == QNetworkConfiguration::UserChoice) {
		id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
	} else {
		id = config.identifier();
	}

	QSettings settings(QSettings::UserScope, QLatin1String("openMittsu"));
	settings.beginGroup(QLatin1String("QtNetwork"));
	settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
	settings.endGroup();

	isNetworkSessionReady = true;

	emit readyConnect();
}

bool ProtocolClient::waitForData(qint64 minBytesRequired) {
	socket->waitForReadyRead(5000);

	if (socket->bytesAvailable() < minBytesRequired) {
		return false;
	} else {
		return true;
	}
}

void ProtocolClient::socketConnected() {
	LOGGER_DEBUG("Socket is now connected.");
	if (isConnected) {
		socketDisconnected(false);
		LOGGER()->warn("Connection is already established, but the socket reconnected?!");
	}

	if (socket->write(cryptoBox.getClientShortTermKeyPair().getPublicKey()) != Key::getPublicKeyLength()) {
		LOGGER()->critical("Could not write the short term public key to server.");
		emit connectToFinished(-5, "Could not write the short term public key to server.");
		return;
	}
	
	QByteArray const clientNoncePrefix(cryptoBox.getClientNonceGenerator().getNoncePrefix());
	if (socket->write(clientNoncePrefix) != clientNoncePrefix.size()) {
		LOGGER()->critical("Could not write the client nonce prefix to server.");
		emit connectToFinished(-6, "Could not write the client nonce prefix to server.");
		return;
	}
	LOGGER_DEBUG("Client Nonce Prefix: ", QString(clientNoncePrefix.toHex()).toStdString());

	socket->flush();
	LOGGER_DEBUG("Wrote Client Hello.");

	// Wait for server answer
	if (!waitForData(PROTO_SERVERHELLO_LENGTH_BYTES)) {
		LOGGER()->critical("Got no reply from server, there are {} of {} bytes available.", socket->bytesAvailable(), PROTO_SERVERHELLO_LENGTH_BYTES);
		emit connectToFinished(-7, "Server did not reply after Client Hello (incorrect IP or port?).");
		return;
	}

	LOGGER_DEBUG("Read {} bytes from server.", socket->bytesAvailable());
	QByteArray const serverHello = socket->read(PROTO_SERVERHELLO_LENGTH_BYTES);
	if (serverHello.size() != (PROTO_SERVERHELLO_LENGTH_BYTES)) {
		LOGGER()->critical("Could not read enough data from server, even though it should be available.");
		emit connectToFinished(-8, "Could not read enough data from server, even though it should be available.");
		return;
	}
	LOGGER_DEBUG("Data (server HELLO): {}", QString(serverHello.toHex()).toStdString());

	cryptoBox.setServerNoncePrefixFromServerHello(serverHello.left(NonceGenerator::getNoncePrefixLength()));

	QByteArray const decodedBox = cryptoBox.decryptFromServer(serverHello.mid(NonceGenerator::getNoncePrefixLength()), serverConfiguration.getServerLongTermPublicKey());

	// First short term public key of the server, than our client nonce prefix for validation.
	cryptoBox.setServerShortTermPublicKey(PublicKey::fromDecodedServerResponse(decodedBox.left(Key::getPublicKeyLength())));
	QByteArray const clientNoncePrefixCopy = decodedBox.mid(Key::getPublicKeyLength(), NonceGenerator::getNoncePrefixLength());

	if (cryptoBox.getClientNonceGenerator().getNoncePrefix() != clientNoncePrefixCopy) {
		LOGGER()->critical("The Server returned a different client nonce prefix: {} vs. {}", QString(cryptoBox.getClientNonceGenerator().getNoncePrefix().toHex()).toStdString(), QString(clientNoncePrefixCopy.toHex()).toStdString());
		emit connectToFinished(-10, "The Server returned a different client nonce prefix");
		return;
	}

	// Send Authentication Package
	QByteArray const authenticationPacket = AuthenticationPacket(clientConfiguration.getClientIdentity(), &cryptoBox).toPacket();

	// Now encrypt the package using the short-term keys
	QByteArray const authenticationPackageEncrypted = cryptoBox.encryptForServer(authenticationPacket);

	// Write authentication package to server
	if (socket->write(authenticationPackageEncrypted) != (crypto_box_MACBYTES + PROTO_AUTHENTICATION_UNENCRYPTED_LENGTH_BYTES)) {
		LOGGER()->critical("Could not write the authentication package to server.");
		emit connectToFinished(-16, "Could not write the authentication package to server.");
		return;
	}
	socket->flush();

	if (!waitForData(PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES)) {
		LOGGER()->critical("Got no reply from server for AuthAck, we have {} of {} bytes available.", socket->bytesAvailable(), PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);
		emit connectToFinished(-17, "Server did not reply after sending client authentication (invalid identity?).");
		return;
	}
	LOGGER_DEBUG("The AuthAck package is {} bytes long (expecting {} bytes).", socket->bytesAvailable(), PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);

	// Decrypt Authentication acknowledgment
	QByteArray authenticationAcknowledgment = socket->read(PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);
	if (authenticationAcknowledgment.size() != (PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES)) {
		LOGGER()->critical("Could not read authentication acknowledgment data from Server, even though it should be available.");
		emit connectToFinished(-18, "Could not read authentication acknowledgment data from Server, even though it should be available.");
		return;
	}
	LOGGER_DEBUG("Data (server authAck): {}", QString(authenticationAcknowledgment.toHex()).toStdString());

	QByteArray authenticationAcknowledgmentDecrypted = cryptoBox.decryptFromServer(authenticationAcknowledgment);

	LOGGER_DEBUG("Handshake finished!");
	emit connectToFinished(0, "Success");
	connectionStart = QDateTime::currentDateTime();
	isConnected = true;
	isAllowedToSend = false;
	readyReadRemainingData.clear();

	// Test if the server already sent a first data package
	if (socket->bytesAvailable() > 0) {
		LOGGER_DEBUG("Socket has {} Bytes available after handshake.", socket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(socketOnReadyRead()));
	}
}

QDateTime const& ProtocolClient::getConnectedSince() const {
	return connectionStart;
}

quint64 ProtocolClient::getReceivedMessagesCount() const {
	return messagesReceived;
}

quint64 ProtocolClient::getSendMessagesCount() const {
	return messagesSend;
}

quint64 ProtocolClient::getReceivedBytesCount() const {
	return bytesReceived;
}

quint64 ProtocolClient::getSendBytesCount() const {
	return bytesSend;
}
