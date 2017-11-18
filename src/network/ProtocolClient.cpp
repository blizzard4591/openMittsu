#include "src/network/ProtocolClient.h"

#include <QMessageBox>
#include <QMutex>
#include <QByteArray>

#include "src/acknowledgments/ContactMessageAcknowledgmentProcessor.h"
#include "src/acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"
#include "src/acknowledgments/GroupCreationMessageAcknowledgmentProcessor.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/ProtocolErrorException.h"
#include "src/tasks/CallbackTask.h"
#include "src/tasks/IdentityReceiverCallbackTask.h"
#include "src/tasks/MessageCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/ByteArrayToHexString.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/OptionMaster.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/ThreadDeleter.h"

#include "src/messages/Message.h"
#include "src/messages/MessageFlagsFactory.h"
#include "src/messages/MessageWithEncryptedPayload.h"
#include "src/messages/MessageWithPayload.h"
#include "src/messages/IncomingMessagesParser.h"
#include "src/messages/contact/ContactMessage.h"
#include "src/messages/contact/ContactImageIdAndKeyMessageContent.h"
#include "src/messages/contact/ContactImageMessageContent.h"
#include "src/messages/group/SpecializedGroupMessage.h"
#include "src/messages/group/UnspecializedGroupMessage.h"
#include "src/messages/group/GroupTextMessageContent.h"
#include "src/messages/group/GroupImageMessageContent.h"
#include "src/protocol/AuthenticationPacket.h"
#include "src/protocol/ClientAcknowledgement.h"
#include "src/crypto/NonceGenerator.h"
#include "src/utility/Endian.h"

#include "src/dataproviders/MessageCenter.h"

#include "sodium.h"

namespace openmittsu {
	namespace network {

		ProtocolClient::ProtocolClient(std::shared_ptr<openmittsu::crypto::FullCryptoBox> cryptoBox, openmittsu::protocol::ContactId const& ourContactId, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster, std::shared_ptr<openmittsu::network::MessageCenterWrapper> const& messageCenterWrapper, openmittsu::protocol::PushFromId const& pushFromId)
			: QObject(nullptr), m_cryptoBox(std::move(cryptoBox)), m_messageCenterWrapper(messageCenterWrapper), m_pushFromIdPtr(std::make_unique<openmittsu::protocol::PushFromId>(pushFromId)),
			m_isSetupDone(false), m_isNetworkSessionReady(false), m_isConnected(false), m_isAllowedToSend(false), m_isDisconnecting(false), m_socket(nullptr), m_networkSession(nullptr), m_ourContactId(ourContactId), m_serverConfiguration(serverConfiguration), m_optionMaster(optionMaster), outgoingMessagesTimer(nullptr), acknowledgmentWaitingTimer(nullptr), keepAliveTimer(nullptr), keepAliveCounter(0), failedReconnectAttempts(0) {
			// Intentionally left empty.
		}

		ProtocolClient::~ProtocolClient() {
			if (m_isConnected || m_isSetupDone) {
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
			if (!m_isSetupDone) {
				throw openmittsu::exceptions::InternalErrorException() << "ProtocolClient::connect() was called before the initial setup was finished!";
			} else if (!m_isNetworkSessionReady) {
				throw openmittsu::exceptions::InternalErrorException() << "ProtocolClient::connect() was called before the network session was available!";
			} else if (m_isConnected) {
				LOGGER()->info("Disconnecting from old server before connection with new settings...");
				disconnectFromServer();
			}
	
			// Clear socket
			m_socket->abort();

			m_isDisconnecting = false;

			// Reset stats
			messagesReceived = 0;
			messagesSend = 0;
			bytesSend = 0;
			bytesReceived = 0;
			//acknowledgmentWaitingMutex.lock();
			//acknowledgmentWaitingMessages.clear();
			//acknowledgmentWaitingMutex.unlock();

			LOGGER()->info("Now connecting to {} on port {}.", m_serverConfiguration->getServerHost().toStdString(), m_serverConfiguration->getServerPort());
			m_socket->connectToHost(m_serverConfiguration->getServerHost(), m_serverConfiguration->getServerPort());
		}

		void ProtocolClient::socketDisconnected(bool emitSignal) {
			LOGGER()->info("Socket is now in state disconnected.");
			m_isConnected = false;
			m_isAllowedToSend = false;
			keepAliveTimer->stop();
			outgoingMessagesTimer->stop();

			if (!m_isDisconnecting && (failedReconnectAttempts < 3) && m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS)) {
				LOGGER()->info("Trying to reconnect...");
				connectToServer();
				return;
			} else if (failedReconnectAttempts >= 3) {
				failedReconnectAttempts = 0;
			}

			if (emitSignal) {
				emit lostConnection();
			}
		}

		bool ProtocolClient::getIsConnected() const {
			return m_isConnected;
		}

		void ProtocolClient::disconnectFromServer() {
			if (getIsConnected()) {
				LOGGER_DEBUG("Closing socket.");
				m_isDisconnecting = true;
				m_socket->close();
			}
		}

		void ProtocolClient::setup() {
			if (!m_isSetupDone) {
				if (m_socket == nullptr) {
					m_socket = std::make_unique<QTcpSocket>();
					if (m_socket == nullptr) {
						return;
					}
				}

				OPENMITTSU_CONNECT(m_socket.get(), readyRead(), this, socketOnReadyRead());
				OPENMITTSU_CONNECT(m_socket.get(), error(QAbstractSocket::SocketError), this, socketOnError(QAbstractSocket::SocketError));
				OPENMITTSU_CONNECT(m_socket.get(), connected(), this, socketConnected());
				OPENMITTSU_CONNECT(m_socket.get(), disconnected(), this, socketDisconnected());

				outgoingMessagesTimer = std::make_unique<QTimer>();
				outgoingMessagesTimer->setInterval(500);
				OPENMITTSU_CONNECT(outgoingMessagesTimer.get(), timeout(), this, outgoingMessagesTimerOnTimer());

				acknowledgmentWaitingTimer = std::make_unique<QTimer>();
				acknowledgmentWaitingTimer->setInterval(5000);
				OPENMITTSU_CONNECT(acknowledgmentWaitingTimer.get(), timeout(), this, acknowledgmentWaitingTimerOnTimer());
				acknowledgmentWaitingTimer->start();

				keepAliveTimer = std::make_unique<QTimer>();
				keepAliveTimer->setInterval(3 * 60 * 1000);
				keepAliveCounter = 1;
				OPENMITTSU_CONNECT(keepAliveTimer.get(), timeout(), this, keepAliveTimerOnTimer());

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

					m_networkSession = std::make_unique<QNetworkSession>(config);
					OPENMITTSU_CONNECT(m_networkSession.get(), opened(), this, networkSessionOnIsOpen());

					m_isNetworkSessionReady = false;
					m_networkSession->open();
				} else {
					LOGGER_DEBUG("Not using the Network Manager and Session.");
					m_isNetworkSessionReady = true;

					emit readyConnect();
				}

				m_isSetupDone = true;
				emit setupDone();
			}
		}

		void ProtocolClient::teardown() {
			if (m_isSetupDone) {
				OPENMITTSU_DISCONNECT(m_socket.get(), readyRead(), this, socketOnReadyRead());
				OPENMITTSU_DISCONNECT(m_socket.get(), error(QAbstractSocket::SocketError), this, socketOnError(QAbstractSocket::SocketError));
				OPENMITTSU_DISCONNECT(m_socket.get(), connected(), this, socketConnected());
				OPENMITTSU_DISCONNECT(m_socket.get(), disconnected(), this, socketDisconnected());

				OPENMITTSU_DISCONNECT(outgoingMessagesTimer.get(), timeout(), this, outgoingMessagesTimerOnTimer());
				OPENMITTSU_DISCONNECT(acknowledgmentWaitingTimer.get(), timeout(), this, acknowledgmentWaitingTimerOnTimer());
				OPENMITTSU_DISCONNECT(keepAliveTimer.get(), timeout(), this, keepAliveTimerOnTimer());

				if (m_isConnected) {
					LOGGER()->info("Disconnecting from Server on teardown.");
					m_isDisconnecting = true;
					m_socket->close();
					m_isConnected = false;
				}

				m_socket = nullptr;

				m_isNetworkSessionReady = false;
				if (m_networkSession != nullptr) {
					OPENMITTSU_DISCONNECT(m_networkSession.get(), opened(), this, networkSessionOnIsOpen());
					m_networkSession = nullptr;
				}

				outgoingMessagesTimer->stop();
				outgoingMessagesTimer = nullptr;
		
				acknowledgmentWaitingTimer->stop();
				acknowledgmentWaitingTimer = nullptr;
		
				keepAliveTimer->stop();
				keepAliveTimer = nullptr;

				m_isSetupDone = false;
			}
			emit teardownComplete();
		}

		void ProtocolClient::outgoingMessagesTimerOnTimer() {
			QMutexLocker lock(&outgoingMessagesMutex);
			if (!m_isConnected || !m_isAllowedToSend) {
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
					quint16 const size = openmittsu::utility::Endian::uint16FromHostEndianToLittleEndian(static_cast<quint16>(data.size()));
					QByteArray const lengthBytes = openmittsu::utility::ByteArrayConversions::convertQuint16toQByteArray(size);
					// First write the two bytes giving the length of the packet
					m_socket->write(lengthBytes);
					// Then write the data itself
					m_socket->write(data);
					m_socket->flush();

					// Update stats
					bytesSend += (2 + data.size());
					messagesSend += 1;

					++count;
				}
				LOGGER_DEBUG("Wrote {} messages to server.", count);
				outgoingMessages.clear();
				outgoingMessagesTimer->stop();
			}
		}

		void ProtocolClient::acknowledgmentWaitingTimerOnTimer() {
			QMutexLocker lock(&acknowledgmentWaitingMutex);
			if (acknowledgmentWaitingMessages.size() > 0) {
				QDateTime const now = QDateTime::currentDateTime();
				QHash<openmittsu::protocol::MessageId, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>>::iterator i = acknowledgmentWaitingMessages.begin();
				QHash<openmittsu::protocol::MessageId, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>>::iterator end = acknowledgmentWaitingMessages.end();
				while (i != end) {
					if (i.value()->getTimeoutTime() <= now) {
						i.value()->sendFailedTimeout(this);

						i = acknowledgmentWaitingMessages.erase(i);
					} else {
						++i;
					}
				}
			}
		}

		void ProtocolClient::keepAliveTimerOnTimer() {
			if (!m_isConnected || !m_isAllowedToSend) {
				keepAliveTimer->stop();
			} else {
				++keepAliveCounter;
				// Send Packet.
				LOGGER_DEBUG("Sending Keep-Alive packet with value {}.", keepAliveCounter);
				uint32_t const keepAliveValue = openmittsu::utility::Endian::uint32FromHostEndianToLittleEndian(keepAliveCounter);

				// Signature Byte: 0x00
				QByteArray packet(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, 0x00);
				packet[0] = (PROTO_PACKET_SIGNATURE_KEEPALIVE_REQUEST);

				packet.append(openmittsu::utility::ByteArrayConversions::convertQuint32toQByteArray(keepAliveValue));
				encryptAndSendDataPacketToServer(packet);
			}
		}

		void ProtocolClient::messageSendFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId) {
			m_messageCenterWrapper->processMessageSendFailed(groupId, messageId);
		}

		void ProtocolClient::messageSendFailed(openmittsu::protocol::ContactId const& contactId, openmittsu::protocol::MessageId const& messageId) {
			m_messageCenterWrapper->processMessageSendFailed(contactId, messageId);
		}

		void ProtocolClient::messageSendDone(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId) {
			m_messageCenterWrapper->processMessageSendDone(groupId, messageId);
		}

		void ProtocolClient::messageSendDone(openmittsu::protocol::ContactId const& contactId, openmittsu::protocol::MessageId const& messageId) {
			m_messageCenterWrapper->processMessageSendDone(contactId, messageId);
		}

		void ProtocolClient::socketOnReadyRead() {
			if (!m_isConnected) {
				// ignore until the handshake is complete.
				return;
			}
			QByteArray newData(m_socket->readAll());
			bytesReceived += newData.size();

			m_readyReadRemainingData.append(newData);
			int bytesAvailable = m_readyReadRemainingData.size();

			if (bytesAvailable < PROTO_DATA_HEADER_SIZE_LENGTH_BYTES) {
				// Not enough data, not even the length Bytes!
				return;
			}

			// Unsigned Two-Byte Integer in Little-Endian
			quint16 packetLength = openmittsu::utility::Endian::uint16FromLittleEndianToHostEndian(openmittsu::utility::ByteArrayConversions::convert2ByteQByteArrayToQuint16(m_readyReadRemainingData.left(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES)));
			if (bytesAvailable < static_cast<int>((PROTO_DATA_HEADER_SIZE_LENGTH_BYTES + packetLength))) {
				// packet not yet complete
				LOGGER_DEBUG("Deferring, packet not yet complete.");
				return;
			}

			// Remove the two length-bytes and packet
			QByteArray const packet = m_readyReadRemainingData.mid(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES, packetLength);
			m_readyReadRemainingData = m_readyReadRemainingData.mid(PROTO_DATA_HEADER_SIZE_LENGTH_BYTES + packetLength, -1);

			QByteArray const decodedPacket = m_cryptoBox->decryptFromServer(packet);

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
				openmittsu::messages::MessageWithEncryptedPayload const messageWithEncryptedPayload(openmittsu::messages::MessageWithEncryptedPayload::fromPacket(decodedPacket));
				//sendClientAcknowlegmentForMessage(messageWithEncryptedPayload);

				handleIncomingMessage(messageWithEncryptedPayload);
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_KEEPALIVE_ANSWER)) {
				LOGGER_DEBUG("Received a KEEP_ALIVE_REPLY packet.");
				handleIncomingKeepAliveAnswer(packetContents);
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_KEEPALIVE_REQUEST)) {
				LOGGER_DEBUG("Received a KEEP_ALIVE_REQUEST packet.");
				handleIncomingKeepAliveRequest(packetContents);
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_SERVER_ACK)) {
				// This should be OUR id. Does not make sense, maybe an early error in the server implementation?
				openmittsu::protocol::ContactId const senderIdentity(packetContents.left(PROTO_IDENTITY_LENGTH_BYTES));
				openmittsu::protocol::MessageId const messageId(packetContents.right(PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES));
				if (senderIdentity != m_ourContactId) {
					LOGGER()->error("Received a SERVER_ACKNOWLEDGE packet for Sender {} and Message #{}, but we are NOT the sender?!", senderIdentity.toString(), messageId.toString());
				} else {
					LOGGER_DEBUG("Received a SERVER_ACKNOWLEDGE packet for message #{}.", messageId.toString());
					handleIncomingAcknowledgment(messageId);
				}
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CLIENT_ACK)) {
				LOGGER()->warn("Received a CLIENT_ACKNOWLEDGE packet: {}", QString(packetContents.toHex()).toStdString());
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CONNECTION_ESTABLISHED)) {
				LOGGER()->info("Received a CONNECTION_ESTABLISHED packet.");
				m_isAllowedToSend = true;
				keepAliveCounter = 1;
				keepAliveTimer->start();
			} else if (packetTypeByte == (PROTO_PACKET_SIGNATURE_CONNECTION_DUPLICATE)) {
				LOGGER()->warn("Received a CONNECTION_DUPLICATE warning, we will be forcefully disconnected after this. Message from Server: {}", QString::fromUtf8(packetContents).toStdString());

				emit duplicateIdUsageDetected();
			} else {
				LOGGER()->warn("Received an UNKNOWN packet with signature {} and payload {}.", QString(decodedPacket.left(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES).toHex()).toStdString(), QString(decodedPacket.toHex()).toStdString());
			}

			// Check if there is more to do
			if (m_readyReadRemainingData.size() > 0) {
				QTimer::singleShot(0, this, SLOT(socketOnReadyRead()));
			}
		}

		void ProtocolClient::handleIncomingAcknowledgment(openmittsu::protocol::MessageId const& messageId) {
			QMutexLocker lock(&acknowledgmentWaitingMutex);
			if (acknowledgmentWaitingMessages.contains(messageId)) {
				QHash<openmittsu::protocol::MessageId, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>>::const_iterator i = acknowledgmentWaitingMessages.constFind(messageId);
				i.value()->sendSuccess(this, messageId);

				if (i.value()->isDone()) {
					acknowledgmentWaitingMessages.remove(messageId);
				}
			} else {
				LOGGER()->warn("Received an incoming acknowledgment for message ID #{}, but no AcknowledgmentProcessor is registered for this message ID.", messageId.toString());
			}
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::MessageWithEncryptedPayload const& message) {
			openmittsu::protocol::ContactId const& receiver = message.getMessageHeader().getReceiver();
			openmittsu::protocol::ContactId const& sender = message.getMessageHeader().getSender();

			if (receiver != m_ourContactId) {
				LOGGER()->critical("Received an incoming text message packet, but we are not the receiver.\nIt was intended for {} from sender {}.", receiver.toString(), sender.toString());
			} else {
				if (needToWaitForMissingIdentity(sender, &message)) {
					return;
				}
		
				openmittsu::messages::MessageWithPayload messageWithPayload(message.decrypt(m_cryptoBox));
				handleIncomingMessage(messageWithPayload, &message);
			}
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::MessageWithPayload const& messageWithPayload, openmittsu::messages::MessageWithEncryptedPayload const*const message) {
			try {
				std::shared_ptr<openmittsu::messages::Message> messageSharedPtr = openmittsu::messages::IncomingMessagesParser::parseMessageWithPayloadToMessage(messageWithPayload);
				openmittsu::messages::Message const* messagePtr = messageSharedPtr.get();
				handleIncomingMessage(messagePtr, message);
			} catch (openmittsu::exceptions::ProtocolErrorException& pee) {
				LOGGER()->warn("Encountered an error while parsing payload of received message from {} with ID {}: {}\nThe payload was: {}", messageWithPayload.getMessageHeader().getSender().toString(), messageWithPayload.getMessageHeader().getMessageId().toString(), pee.what(), QString(messageWithPayload.getPayload().toHex()).toStdString());
			} catch (std::exception& e) {
				LOGGER()->critical("Unknown error while parsing payload of received message from {} with ID {}: {}", messageWithPayload.getMessageHeader().getSender().toString(), messageWithPayload.getMessageHeader().getMessageId().toString(), e.what());
			}
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::Message const*const message, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload) {
			if (dynamic_cast<openmittsu::messages::contact::ContactMessage const*>(message) != nullptr) {
				openmittsu::messages::contact::ContactMessage const* contactMessage = dynamic_cast<openmittsu::messages::contact::ContactMessage const*>(message);
				openmittsu::messages::contact::ContactMessageContent const* cmc = contactMessage->getContactMessageContent();
				if (cmc->hasPostReceiveCallbackTask()) {
					openmittsu::tasks::CallbackTask* callbackTask = cmc->getPostReceiveCallbackTask(new openmittsu::messages::contact::ContactMessage(*contactMessage), m_serverConfiguration, m_cryptoBox);

					enqeueCallbackTask(callbackTask);
					return;
				}

				if (dynamic_cast<openmittsu::messages::contact::ContactTextMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::contact::ContactTextMessageContent const>(dynamic_cast<openmittsu::messages::contact::ContactTextMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::contact::ContactImageMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::contact::ContactImageMessageContent const>(dynamic_cast<openmittsu::messages::contact::ContactImageMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::contact::ContactLocationMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::contact::ContactLocationMessageContent const>(dynamic_cast<openmittsu::messages::contact::ContactLocationMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::contact::ReceiptMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::contact::ReceiptMessageContent const>(dynamic_cast<openmittsu::messages::contact::ReceiptMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::contact::UserTypingMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(contactMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::contact::UserTypingMessageContent const>(dynamic_cast<openmittsu::messages::contact::UserTypingMessageContent const*>(cmc->clone())));
				} else {
					LOGGER()->critical("No match for content of message from {}.", contactMessage->getMessageHeader().getSender().toString());
				}
			} else if (dynamic_cast<openmittsu::messages::group::SpecializedGroupMessage const*>(message) != nullptr) {
				openmittsu::messages::group::SpecializedGroupMessage const* groupMessage = dynamic_cast<openmittsu::messages::group::SpecializedGroupMessage const*>(message);
				openmittsu::messages::group::GroupMessageContent const* cmc = groupMessage->getGroupMessageContent();

				bool const isGroupCreationMessage = (dynamic_cast<openmittsu::messages::group::GroupCreationMessageContent const*>(cmc) != nullptr);
				openmittsu::protocol::GroupId const groupId = cmc->getGroupId();
		
				if (needToWaitForGroupData(groupId, messageWithEncryptedPayload, isGroupCreationMessage)) {
					return;
				}

				if (cmc->hasPostReceiveCallbackTask()) {
					openmittsu::tasks::CallbackTask* callbackTask = cmc->getPostReceiveCallbackTask(new openmittsu::messages::group::SpecializedGroupMessage(*groupMessage), m_serverConfiguration, m_cryptoBox);

					enqeueCallbackTask(callbackTask);
					return;
				}

				if (dynamic_cast<openmittsu::messages::group::GroupTextMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupTextMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupTextMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupImageMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupImageMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupImageMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupLocationMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupLocationMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupLocationMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupFileMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupFileMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupFileMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupCreationMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupCreationMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupCreationMessageContent const*>(cmc->clone())), messageWithEncryptedPayload);
				} else if (dynamic_cast<openmittsu::messages::group::GroupSetTitleMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupSetTitleMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupSetTitleMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupSetPhotoMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupSetPhotoMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupSetPhotoMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupSyncMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupSyncMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupSyncMessageContent const*>(cmc->clone())));
				} else if (dynamic_cast<openmittsu::messages::group::GroupLeaveMessageContent const*>(cmc) != nullptr) {
					handleIncomingMessage(groupMessage->getMessageHeader(), std::shared_ptr<openmittsu::messages::group::GroupLeaveMessageContent const>(dynamic_cast<openmittsu::messages::group::GroupLeaveMessageContent const*>(cmc->clone())));
				} else {
					LOGGER()->critical("No match for content of group message from {}.", groupMessage->getMessageHeader().getSender().toString());
				}
			} else {
				LOGGER()->critical("Not a contact message, failed to parse message from {}.", message->getMessageHeader().getSender().toString());
			}
		}

		bool ProtocolClient::needToWaitForMissingIdentity(openmittsu::protocol::ContactId const& contactId, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload) {
			if (missingIdentityProcessors.contains(contactId)) {
				LOGGER()->info("Enqueing incoming message on existing MissingIdentityProcessor for ID {}.", contactId.toString());
				missingIdentityProcessors.constFind(contactId).value()->enqueueMessage(*messageWithEncryptedPayload);
				return true;
			} else if ((!m_cryptoBox->getKeyRegistry().hasIdentity(contactId))) {
				std::shared_ptr<MissingIdentityProcessor> missingIdentityProcessor = std::make_shared<MissingIdentityProcessor>(contactId);
				missingIdentityProcessor->enqueueMessage(*messageWithEncryptedPayload);
				missingIdentityProcessors.insert(contactId, missingIdentityProcessor);
				LOGGER()->info("Enqueing MissingIdentityProcessor for ID {}.", contactId.toString());

				openmittsu::tasks::CallbackTask* callbackTask = new openmittsu::tasks::IdentityReceiverCallbackTask(m_serverConfiguration, contactId);

				enqeueCallbackTask(callbackTask);
				return true;
			}

			return false;
		}

		bool ProtocolClient::needToWaitForGroupData(openmittsu::protocol::GroupId const& groupId, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload, bool isGroupCreationMessage) {
			if (needToWaitForMissingIdentity(groupId.getOwner(), messageWithEncryptedPayload)) {
				return true;
			} else if (groupsWithMissingIdentities.contains(groupId)) {
				if (messageWithEncryptedPayload == nullptr) {
					LOGGER()->warn("Trying to enqueue new message for group {} on existing MissingIdentityProcessor, but the encryptedPayload pointer is null.", groupId.toString());
					return true;
				}

				LOGGER_DEBUG("Enqueuing new message for group {} on existing MissingIdentityProcessor.", groupId.toString());
				groupsWithMissingIdentities.constFind(groupId).value()->enqueueMessage(*messageWithEncryptedPayload);
				return true;
			}

			return false;
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactTextMessageContent const> contactTextMessageContent) {
			LOGGER_DEBUG("Received a message from {} with {} Bytes of text.", messageHeader.getSender().toString(), contactTextMessageContent->getText().size());
			m_messageCenterWrapper->processReceivedContactMessageText(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), contactTextMessageContent->getText());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupTextMessageContent const> groupTextMessageContent) {
			LOGGER_DEBUG("Received a group message from {} to group {} with {} Bytes of text.", messageHeader.getSender().toString(), groupTextMessageContent->getGroupId().toString(), groupTextMessageContent->getText().size());
			m_messageCenterWrapper->processReceivedGroupMessageText(groupTextMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupTextMessageContent->getText());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactImageMessageContent const> contactImageMessageContent) {
			LOGGER_DEBUG("Received an image message from {}.", messageHeader.getSender().toString());
			m_messageCenterWrapper->processReceivedContactMessageImage(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), contactImageMessageContent->getImageData());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupImageMessageContent const> groupImageMessageContent) {
			LOGGER_DEBUG("Received an image message from {} to group {}.", messageHeader.getSender().toString(), groupImageMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupMessageImage(groupImageMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupImageMessageContent->getImageData());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ContactLocationMessageContent const> contactLocationMessageContent) {
			LOGGER_DEBUG("Received a location message from {}.", messageHeader.getSender().toString());
			m_messageCenterWrapper->processReceivedContactMessageLocation(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), contactLocationMessageContent->getLocation());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupLocationMessageContent const> groupLocationMessageContent) {
			LOGGER_DEBUG("Received a location message from {} to group {}.", messageHeader.getSender().toString(), groupLocationMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupMessageLocation(groupLocationMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupLocationMessageContent->getLocation());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupFileMessageContent const> groupFileMessageContent) {
			LOGGER()->warn("Received a file message from {} to group {}. THIS IS STILL UNSUPPORTED.", messageHeader.getSender().toString(), groupFileMessageContent->getGroupId().toString());
			sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupCreationMessageContent const> groupCreationMessageContent, openmittsu::messages::MessageWithEncryptedPayload const*const messageWithEncryptedPayload) {
			QSet<openmittsu::protocol::ContactId> const groupMembers = groupCreationMessageContent->getGroupMembers();
			auto it = groupMembers.constBegin();
			auto const end = groupMembers.constEnd();

			QSet<openmittsu::protocol::ContactId> missingIds;
			for (; it != end; ++it) {
				if (missingIdentityProcessors.contains(*it)) {
					if (messageWithEncryptedPayload == nullptr) {
						LOGGER()->warn("Trying to handle GroupCreationMessage for group {} on existing MissingIdentityProcessor, but the encryptedPayload pointer is null.", groupCreationMessageContent->getGroupId().toString());
						return;
					}

					LOGGER_DEBUG("Enqueing group creation message into existing MissingIdentityProcessor for ID {}.", it->toString());
					missingIdentityProcessors.constFind(*it).value()->enqueueMessage(*messageWithEncryptedPayload);
					return;
				} else if (!m_cryptoBox->getKeyRegistry().hasIdentity(*it)) {
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
				auto itMissing = missingIds.constBegin();
				auto const endMissing = missingIds.constEnd();
				for (; itMissing != endMissing; ++itMissing) {
					LOGGER()->info("Enqueing MissingIdentityProcessor for group member with ID {}.", itMissing->toString());
					missingIdentityProcessors.insert(*itMissing, missingIdentityProcessor);

					openmittsu::tasks::CallbackTask* callbackTask = new openmittsu::tasks::IdentityReceiverCallbackTask(m_serverConfiguration, *itMissing);
					enqeueCallbackTask(callbackTask);
				}
				return;
			}

			m_messageCenterWrapper->processReceivedGroupCreation(groupCreationMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupCreationMessageContent->getGroupMembers());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSetPhotoMessageContent const> groupSetPhotoMessageContent) {
			LOGGER_DEBUG("Received a group SetPhoto message from {} to group {}.", messageHeader.getSender().toString(), groupSetPhotoMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupSetImage(groupSetPhotoMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupSetPhotoMessageContent->getGroupPhoto());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSetTitleMessageContent const> groupSetTitleMessageContent) {
			LOGGER_DEBUG("Received a group SetTitle message from {} to group {}.", messageHeader.getSender().toString(), groupSetTitleMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupSetTitle(groupSetTitleMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now(), groupSetTitleMessageContent->getTitle());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupSyncMessageContent const> groupSyncMessageContent) {
			LOGGER_DEBUG("Received a group SyncRequest message from {} to group {}.", messageHeader.getSender().toString(), groupSyncMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupSyncRequest(groupSyncMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::group::GroupLeaveMessageContent const> groupLeaveMessageContent) {
			LOGGER_DEBUG("Received a group Leave message from {} to group {}.", messageHeader.getSender().toString(), groupLeaveMessageContent->getGroupId().toString());
			m_messageCenterWrapper->processReceivedGroupLeave(groupLeaveMessageContent->getGroupId(), messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), openmittsu::protocol::MessageTime::now());
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::ReceiptMessageContent const> receiptMessageContent) {
			openmittsu::messages::contact::ReceiptMessageContent::ReceiptType receiptType = receiptMessageContent->getReceiptType();
			std::vector<openmittsu::protocol::MessageId> const refMessageIds = receiptMessageContent->getReferredMessageIds();

			for (openmittsu::protocol::MessageId refMessageId: refMessageIds) {
				switch (receiptType) {
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::RECEIVED:
					LOGGER_DEBUG("Received Receipt: User {} has received message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
					m_messageCenterWrapper->processReceivedContactMessageReceiptReceived(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), refMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::SEEN:
					LOGGER_DEBUG("Received Receipt: User {} has seen message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
					m_messageCenterWrapper->processReceivedContactMessageReceiptSeen(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), refMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::AGREE:
					LOGGER_DEBUG("Received Receipt: User {} has agreed with message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
					m_messageCenterWrapper->processReceivedContactMessageReceiptAgree(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), refMessageId);
					break;
				case openmittsu::messages::contact::ReceiptMessageContent::ReceiptType::DISAGREE:
					LOGGER_DEBUG("Received Receipt: User {} has disagreed with message #{}.", messageHeader.getSender().toString(), refMessageId.toString());
					m_messageCenterWrapper->processReceivedContactMessageReceiptDisagree(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime(), refMessageId);
					break;
				default:
					LOGGER()->error("Unknown ReceiptType \"{}\" in handleIncomingMessage(receiptMessageContent)!", static_cast<int>(receiptType));
					sendMessageReceivedAcknowledgement(messageHeader.getSender(), messageHeader.getMessageId());
					break;
				}
			}
		}

		void ProtocolClient::handleIncomingMessage(openmittsu::messages::FullMessageHeader const& messageHeader, std::shared_ptr<openmittsu::messages::contact::UserTypingMessageContent const> userTypingMessageContent) {
			if (userTypingMessageContent->isUserTyping()) {
				m_messageCenterWrapper->processReceivedContactTypingNotificationTyping(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime());
			} else {
				m_messageCenterWrapper->processReceivedContactTypingNotificationStopped(messageHeader.getSender(), messageHeader.getMessageId(), messageHeader.getTime());
			}
		}

		void ProtocolClient::handleIncomingKeepAliveRequest(QByteArray const& packetData) {
			if (packetData.size() > (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES)) {
				throw openmittsu::exceptions::ProtocolErrorException() << "Invalid KeepAliveRequest packet from the server, size mismatch (" << packetData.size() << " > " << (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES) << " Bytes).";
			}

			LOGGER_DEBUG("Sending reply to Server KEEP_ALIVE_REQUEST with {} Bytes.", packetData.size());
			QByteArray packet(PROTO_DATA_HEADER_TYPE_LENGTH_BYTES, 0x00);
			packet[0] = (PROTO_PACKET_SIGNATURE_KEEPALIVE_ANSWER);

			packet.append(packetData);
			encryptAndSendDataPacketToServer(packet);
		}

		void ProtocolClient::handleIncomingKeepAliveAnswer(QByteArray const& packetData) {
			if (packetData.size() > (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES)) {
				throw openmittsu::exceptions::ProtocolErrorException() << "Invalid KeepAlive packet from the server, size mismatch (" << packetData.size() << " > " << (PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES) << " Bytes).";
			}
	
			if (packetData.size() == sizeof(keepAliveCounter)) {
				quint32 const keepAliveCounterReplyValue = openmittsu::utility::Endian::uint32FromLittleEndianToHostEndian(openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(packetData));
				if (keepAliveCounterReplyValue != keepAliveCounter) {
					LOGGER()->warn("The keep-alive reply is not identical to what we send out. It should be {}, but it is {}.", keepAliveCounter, keepAliveCounterReplyValue);
				}
			} else {
				LOGGER()->warn("Received a keep alive reply with a size that does not match the size we send out ({} Bytes vs. {} Bytes).", packetData.size(), sizeof(keepAliveCounter));
			}
		}

		void ProtocolClient::sendContactMessage(openmittsu::messages::contact::PreliminaryContactMessage const& preliminaryMessage) {
			if (preliminaryMessage.getPreliminaryMessageHeader()->getReceiver() == m_ourContactId) {
				return;
			}
	
			openmittsu::messages::contact::ContactMessage contactMessage(openmittsu::messages::FullMessageHeader(preliminaryMessage.getPreliminaryMessageHeader(), m_ourContactId, *m_pushFromIdPtr), preliminaryMessage.getPreliminaryMessageContent()->clone());
			std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> ap = std::make_shared<openmittsu::acknowledgments::ContactMessageAcknowledgmentProcessor>(contactMessage.getMessageHeader().getReceiver(), QDateTime::currentDateTime().addSecs(15), contactMessage.getMessageHeader().getMessageId());
			handleOutgoingMessage(&contactMessage, ap);
		}

		void ProtocolClient::handleOutgoingMessage(openmittsu::messages::contact::ContactMessage const*const contactMessage, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor) {
			if (contactMessage->getContactMessageContent()->hasPreSendCallbackTask()) {
				openmittsu::tasks::CallbackTask* callbackTask = contactMessage->getContactMessageContent()->getPreSendCallbackTask(new openmittsu::messages::contact::ContactMessage(*contactMessage), acknowledgmentProcessor, m_serverConfiguration, m_cryptoBox);

				enqeueCallbackTask(callbackTask);
				return;
			}

			LOGGER_DEBUG("Sending Message to Contact {} with ID {}.", contactMessage->getMessageHeader().getReceiver().toString(), contactMessage->getMessageHeader().getMessageId().toString());

			openmittsu::messages::MessageWithPayload messageWithPayload(contactMessage->getMessageHeader(), contactMessage->getContactMessageContent()->toPacketPayload());
			openmittsu::messages::MessageWithEncryptedPayload messageWithEncryptedPayload(messageWithPayload.encrypt(m_cryptoBox));

			encryptAndSendDataPacketToServer(messageWithEncryptedPayload.toPacket());

			if (!contactMessage->getMessageHeader().getFlags().isNoAckExpectedForMessage()) {
				enqeueWaitForAcknowledgment(contactMessage->getMessageHeader().getReceiver(), contactMessage->getMessageHeader().getMessageId(), acknowledgmentProcessor);
			}
		}

		void ProtocolClient::handleOutgoingMessage(openmittsu::messages::group::UnspecializedGroupMessage const*const message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor) {
			if (message->getGroupMessageContent()->hasPreSendCallbackTask()) {
				openmittsu::tasks::CallbackTask* callbackTask = message->getGroupMessageContent()->getPreSendCallbackTask(new openmittsu::messages::group::UnspecializedGroupMessage(*message), acknowledgmentProcessor, m_serverConfiguration, m_cryptoBox);

				enqeueCallbackTask(callbackTask);
				return;
			}

			QSet<openmittsu::protocol::ContactId>::const_iterator it = message->getRecipients().constBegin();
			QSet<openmittsu::protocol::ContactId>::const_iterator end = message->getRecipients().constEnd();
			for (; it != end; ++it) {
				if (*it != m_ourContactId) {
					openmittsu::messages::group::SpecializedGroupMessage groupMessageForSingleContact(*message, *it, message->getMessageHeader().getMessageId());

					handleOutgoingMessage(&groupMessageForSingleContact, acknowledgmentProcessor);
				}
			}
		}

		void ProtocolClient::handleOutgoingMessage(openmittsu::messages::group::SpecializedGroupMessage const*const message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor) {
			LOGGER_DEBUG("Sending GroupMessage to Contact {} with ID {}.", message->getMessageHeader().getReceiver().toString(), message->getMessageHeader().getMessageId().toString());

			openmittsu::messages::MessageWithPayload messageWithPayload(message->getMessageHeader(), message->getGroupMessageContent()->toPacketPayload());
			openmittsu::messages::MessageWithEncryptedPayload messageWithEncryptedPayload(messageWithPayload.encrypt(m_cryptoBox));

			encryptAndSendDataPacketToServer(messageWithEncryptedPayload.toPacket());

			if (!message->getMessageHeader().getFlags().isNoAckExpectedForMessage()) {
				enqeueWaitForAcknowledgment(message->getMessageHeader().getReceiver(), message->getMessageHeader().getMessageId(), acknowledgmentProcessor);
			}
		}

		void ProtocolClient::sendGroupMessage(openmittsu::messages::group::PreliminaryGroupMessage const& preliminaryMessage) {
			openmittsu::protocol::GroupId const& groupId = preliminaryMessage.getPreliminaryMessageHeader()->getGroup();
			openmittsu::messages::group::UnspecializedGroupMessage groupMessage(preliminaryMessage, m_ourContactId, *m_pushFromIdPtr);
			std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> ap = std::make_shared<openmittsu::acknowledgments::GroupContentMessageAcknowledgmentProcessor>(groupId, QDateTime::currentDateTime().addSecs(15), groupMessage.getMessageHeader().getMessageId());
			handleOutgoingMessage(&groupMessage, ap);
		}

		void ProtocolClient::newPushFromId(openmittsu::protocol::PushFromId const& newPushFromId) {
			LOGGER_DEBUG("Setting new PushFromId {} received from GUI.", newPushFromId.toString());
			m_pushFromIdPtr = std::make_unique<openmittsu::protocol::PushFromId>(newPushFromId);
		}

		void ProtocolClient::enqeueCallbackTask(openmittsu::tasks::CallbackTask* callbackTask) {
			if (callbackTask == nullptr) {
				LOGGER()->warn("Ignoring nullptr callback task.");
				return;
			}

			OPENMITTSU_CONNECT_QUEUED(callbackTask, finished(openmittsu::tasks::CallbackTask*), this, callbackTaskFinished(openmittsu::tasks::CallbackTask*));

			QMetaObject::invokeMethod(callbackTask, "start", Qt::QueuedConnection);
		}

		void ProtocolClient::callbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask) {
			if (dynamic_cast<openmittsu::tasks::IdentityReceiverCallbackTask*>(callbackTask) != nullptr) {
				openmittsu::utility::UniquePtrWithDelayedThreadDeletion<openmittsu::tasks::IdentityReceiverCallbackTask> irct(dynamic_cast<openmittsu::tasks::IdentityReceiverCallbackTask*>(callbackTask));
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
						if (m_cryptoBox->getKeyRegistry().hasIdentity(irct->getContactIdOfFetchedPublicKey())) {
							LOGGER()->warn("Identity {} is already known to KeyRegistry, ignoring result of IdentityReceiverCallbackTask.", irct->getContactIdOfFetchedPublicKey().toString());
						} else {
							// TODO FIXME: Add new identity to database!
							//m_cryptoBox->getKeyRegistry().addIdentity(irct->getContactIdOfFetchedPublicKey(), irct->getFetchedPublicKey());

							LOGGER_DEBUG("PublicKey for Identity {} is {}.", irct->getContactIdOfFetchedPublicKey().toString(), irct->getFetchedPublicKey().toString());
							m_messageCenterWrapper->addNewContact(irct->getContactIdOfFetchedPublicKey(), irct->getFetchedPublicKey());
						}
					}

					if (missingIdentityProcessor->hasFinished()) {
						if (missingIdentityProcessor->hasAssociatedGroupId()) {
							groupsWithMissingIdentities.remove(missingIdentityProcessor->getAssociatedGroupId());
						}
						if (missingIdentityProcessor->hasFinishedSuccessfully()) {
							LOGGER()->info("MissingIdentityProcessor finished successfully, now processing {} queued messages.", missingIdentityProcessor->getQueuedMessages().size());
							std::list<openmittsu::messages::MessageWithEncryptedPayload> queuedMessages(missingIdentityProcessor->getQueuedMessages());
							auto it = queuedMessages.cbegin();
							auto const end = queuedMessages.cend();
							for (; it != end; ++it) {
								handleIncomingMessage(*it);
							}
						} else {
							LOGGER()->warn("MissingIdentityProcessor failed, will now discard {} messages.", missingIdentityProcessor->getQueuedMessages().size());
						}
					}
				}
			} else if (dynamic_cast<openmittsu::tasks::MessageCallbackTask*>(callbackTask) != nullptr) {
				openmittsu::utility::UniquePtrWithDelayedThreadDeletion<openmittsu::tasks::MessageCallbackTask> messageCallbackTask(dynamic_cast<openmittsu::tasks::MessageCallbackTask*>(callbackTask));
				if (messageCallbackTask->getInitialMessage()->getMessageHeader().getSender() == m_ourContactId) {
					// Sending
					if (!messageCallbackTask->hasFinishedSuccessfully()) {
						LOGGER()->warn("Tried sending a message to user {} that triggered a Callback Task which did not succeed. Error: {}", messageCallbackTask->getInitialMessage()->getMessageHeader().getReceiver().toString(), messageCallbackTask->getErrorMessage().toStdString());
						messageCallbackTask->getAcknowledgmentProcessor()->sendFailed(this, messageCallbackTask->getInitialMessage()->getMessageHeader().getMessageId());
					} else {
						openmittsu::messages::Message* nextMessage = messageCallbackTask->getResultMessage();
						if (dynamic_cast<openmittsu::messages::contact::ContactMessage*>(nextMessage) != nullptr) {
							handleOutgoingMessage(dynamic_cast<openmittsu::messages::contact::ContactMessage*>(nextMessage), messageCallbackTask->getAcknowledgmentProcessor());
						} else if (dynamic_cast<openmittsu::messages::group::UnspecializedGroupMessage*>(nextMessage) != nullptr) {
							handleOutgoingMessage(dynamic_cast<openmittsu::messages::group::UnspecializedGroupMessage*>(nextMessage), messageCallbackTask->getAcknowledgmentProcessor());
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
						openmittsu::messages::Message* nextMessage = messageCallbackTask->getResultMessage();
						handleIncomingMessage(nextMessage, nullptr);

						delete nextMessage;
					}
				}
			} else {
				LOGGER()->warn("Unhandled callback task of unknown type?!");
				delete callbackTask;
			}
		}

		void ProtocolClient::sendClientAcknowlegmentForMessage(openmittsu::messages::MessageWithEncryptedPayload const& message) {
			LOGGER()->error("sendClientAcknowlegmentForMessage() should not be called anymore?!");
			LOGGER_DEBUG("Sending client acknowledgment to server for message #{}.", message.getMessageHeader().getMessageId().toString());
			encryptAndSendDataPacketToServer(openmittsu::protocol::ClientAcknowledgement(message.getMessageHeader().getSender(), message.getMessageHeader().getMessageId()).toPacket());
		}

		void ProtocolClient::sendMessageReceivedAcknowledgement(openmittsu::protocol::ContactId const& messageSender, openmittsu::protocol::MessageId const& messageId) {
			LOGGER_DEBUG("Sending client acknowledgment to server for message #{} from contact {}.", messageId.toString(), messageSender.toString());
			encryptAndSendDataPacketToServer(openmittsu::protocol::ClientAcknowledgement(messageSender, messageId).toPacket());
		}

		void ProtocolClient::encryptAndSendDataPacketToServer(QByteArray const& dataPacket) {
			QByteArray const encryptedDataPacket = m_cryptoBox->encryptForServer(dataPacket);
			LOGGER_DEBUG("Writing Message with {} Bytes to outbound queue.", encryptedDataPacket.size());

			QMutexLocker lock(&outgoingMessagesMutex);
			outgoingMessages.append(encryptedDataPacket);
			outgoingMessagesTimer->start(0);
		}

		void ProtocolClient::enqeueWaitForAcknowledgment(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor) {
			QMutexLocker lock(&acknowledgmentWaitingMutex);
	
			acknowledgmentProcessor->addMessage(messageId);
			acknowledgmentWaitingMessages.insert(messageId, acknowledgmentProcessor);
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
				LOGGER_DEBUG("SocketError: {}", m_socket->errorString().toStdString());
				emit connectToFinished(-4, tr("The following error occurred: %1.").arg(m_socket->errorString()));
			}
		}

		void ProtocolClient::networkSessionOnIsOpen() {
			// Save the used configuration
			QNetworkConfiguration config = m_networkSession->configuration();
			QString id;
			if (config.type() == QNetworkConfiguration::UserChoice) {
				id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
			} else {
				id = config.identifier();
			}

			QSettings settings(QSettings::UserScope, QLatin1String("openMittsu"));
			settings.beginGroup(QLatin1String("QtNetwork"));
			settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
			settings.endGroup();

			m_isNetworkSessionReady = true;

			emit readyConnect();
		}

		bool ProtocolClient::waitForData(qint64 minBytesRequired) {
			m_socket->waitForReadyRead(5000);

			if (m_socket->bytesAvailable() < minBytesRequired) {
				return false;
			} else {
				return true;
			}
		}

		void ProtocolClient::socketConnected() {
			LOGGER_DEBUG("Socket is now connected.");
			if (m_isConnected) {
				socketDisconnected(false);
				LOGGER()->warn("Connection is already established, but the socket reconnected?!");
			}

			if (m_socket->write(m_cryptoBox->getClientShortTermKeyPair().getPublicKey()) != openmittsu::crypto::Key::getPublicKeyLength()) {
				LOGGER()->critical("Could not write the short term public key to server.");
				++failedReconnectAttempts;
				emit connectToFinished(-5, "Could not write the short term public key to server.");
				return;
			}
	
			QByteArray const clientNoncePrefix(m_cryptoBox->getClientNonceGenerator().getNoncePrefix());
			if (m_socket->write(clientNoncePrefix) != clientNoncePrefix.size()) {
				LOGGER()->critical("Could not write the client nonce prefix to server.");
				++failedReconnectAttempts;
				emit connectToFinished(-6, "Could not write the client nonce prefix to server.");
				return;
			}
			LOGGER_DEBUG("Client Nonce Prefix: {}", QString(clientNoncePrefix.toHex()).toStdString());

			m_socket->flush();
			LOGGER_DEBUG("Wrote Client Hello.");

			// Wait for server answer
			if (!waitForData(PROTO_SERVERHELLO_LENGTH_BYTES)) {
				LOGGER()->critical("Got no reply from server, there are {} of {} bytes available.", m_socket->bytesAvailable(), PROTO_SERVERHELLO_LENGTH_BYTES);
				++failedReconnectAttempts;
				emit connectToFinished(-7, "Server did not reply after Client Hello (incorrect IP or port?).");
				return;
			}

			LOGGER_DEBUG("Read {} bytes from server.", m_socket->bytesAvailable());
			QByteArray const serverHello = m_socket->read(PROTO_SERVERHELLO_LENGTH_BYTES);
			if (serverHello.size() != (PROTO_SERVERHELLO_LENGTH_BYTES)) {
				LOGGER()->critical("Could not read enough data from server, even though it should be available.");
				++failedReconnectAttempts;
				emit connectToFinished(-8, "Could not read enough data from server, even though it should be available.");
				return;
			}
			LOGGER_DEBUG("Data (server HELLO): {}", QString(serverHello.toHex()).toStdString());

			m_cryptoBox->setServerNoncePrefixFromServerHello(serverHello.left(openmittsu::crypto::NonceGenerator::getNoncePrefixLength()));

			QByteArray const decodedBox = m_cryptoBox->decryptFromServer(serverHello.mid(openmittsu::crypto::NonceGenerator::getNoncePrefixLength()), m_serverConfiguration->getServerLongTermPublicKey());

			// First short term public key of the server, than our client nonce prefix for validation.
			m_cryptoBox->setServerShortTermPublicKey(openmittsu::crypto::PublicKey::fromDecodedServerResponse(decodedBox.left(openmittsu::crypto::Key::getPublicKeyLength())));
			QByteArray const clientNoncePrefixCopy = decodedBox.mid(openmittsu::crypto::Key::getPublicKeyLength(), openmittsu::crypto::NonceGenerator::getNoncePrefixLength());

			if (m_cryptoBox->getClientNonceGenerator().getNoncePrefix() != clientNoncePrefixCopy) {
				LOGGER()->critical("The Server returned a different client nonce prefix: {} vs. {}", QString(m_cryptoBox->getClientNonceGenerator().getNoncePrefix().toHex()).toStdString(), QString(clientNoncePrefixCopy.toHex()).toStdString());
				++failedReconnectAttempts;
				emit connectToFinished(-10, "The Server returned a different client nonce prefix");
				return;
			}

			// Send Authentication Package
			QByteArray const authenticationPacket = openmittsu::protocol::AuthenticationPacket(m_ourContactId, m_cryptoBox).toPacket();

			// Now encrypt the package using the short-term keys
			QByteArray const authenticationPackageEncrypted = m_cryptoBox->encryptForServer(authenticationPacket);

			// Write authentication package to server
			if (m_socket->write(authenticationPackageEncrypted) != (crypto_box_MACBYTES + PROTO_AUTHENTICATION_UNENCRYPTED_LENGTH_BYTES)) {
				LOGGER()->critical("Could not write the authentication package to server.");
				++failedReconnectAttempts;
				emit connectToFinished(-16, "Could not write the authentication package to server.");
				return;
			}
			m_socket->flush();

			if (!waitForData(PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES)) {
				LOGGER()->critical("Got no reply from server for AuthAck, we have {} of {} bytes available.", m_socket->bytesAvailable(), PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);
				++failedReconnectAttempts;
				emit connectToFinished(-17, "Server did not reply after sending client authentication (invalid identity?).");
				return;
			}
			LOGGER_DEBUG("The AuthAck package is {} bytes long (expecting {} bytes).", m_socket->bytesAvailable(), PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);

			// Decrypt Authentication acknowledgment
			QByteArray authenticationAcknowledgment = m_socket->read(PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES);
			if (authenticationAcknowledgment.size() != (PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES)) {
				LOGGER()->critical("Could not read authentication acknowledgment data from Server, even though it should be available.");
				++failedReconnectAttempts;
				emit connectToFinished(-18, "Could not read authentication acknowledgment data from Server, even though it should be available.");
				return;
			}
			LOGGER_DEBUG("Data (server authAck): {}", QString(authenticationAcknowledgment.toHex()).toStdString());

			QByteArray authenticationAcknowledgmentDecrypted = m_cryptoBox->decryptFromServer(authenticationAcknowledgment);

			LOGGER_DEBUG("Handshake finished!");
			failedReconnectAttempts = 0;
			connectionStart = QDateTime::currentDateTime();
			m_isConnected = true;
			m_isAllowedToSend = false;
			m_readyReadRemainingData.clear();

			// Test if the server already sent a first data package
			if (m_socket->bytesAvailable() > 0) {
				LOGGER_DEBUG("Socket has {} Bytes available after handshake.", m_socket->bytesAvailable());
				QTimer::singleShot(0, this, SLOT(socketOnReadyRead()));
			}

			emit connectToFinished(0, "Success");
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

	}

}
