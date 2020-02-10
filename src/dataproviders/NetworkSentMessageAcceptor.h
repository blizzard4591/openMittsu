#ifndef OPENMITTSU_DATABASE_NETWORKSENTMESSAGEACCEPTOR_H_
#define OPENMITTSU_DATABASE_NETWORKSENTMESSAGEACCEPTOR_H_

#include "src/dataproviders/SentMessageAcceptor.h"

#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/messages/group/PreliminaryGroupMessage.h"

#include <QObject>

#include <memory>

namespace openmittsu {
	namespace network {
		class ProtocolClient;
	}

	namespace test {
		class MockNetworkSentMessageAcceptor;
	}

	namespace dataproviders {

		class NetworkSentMessageAcceptor : public QObject, public SentMessageAcceptor {
			Q_OBJECT
		public:
			NetworkSentMessageAcceptor(std::weak_ptr<openmittsu::network::ProtocolClient> const& protocolClient);
			virtual ~NetworkSentMessageAcceptor();

		public slots:
			virtual void processSentContactMessageAudio(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void processSentContactMessageFile(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void processSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) override;
			virtual void processSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) override;
			virtual void processSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) override;
			virtual void processSentContactMessageVideo(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;

			virtual void processSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;
			virtual void processSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) override;

			virtual void processSentContactMessageTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void processSentContactMessageTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;

			virtual void processSentGroupMessageAudio(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& audio, quint16 lengthInSeconds) override;
			virtual void processSentGroupMessageFile(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) override;
			virtual void processSentGroupMessageImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image, QString const& caption) override;
			virtual void processSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::utility::Location const& location) override;
			virtual void processSentGroupMessageText(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& message) override;
			virtual void processSentGroupMessageVideo(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) override;

			virtual void processSentGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual void processSentGroupSetImage(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QByteArray const& image) override;
			virtual void processSentGroupSetTitle(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, QString const& groupTitle) override;
			virtual void processSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) override;
			virtual void processSentGroupLeave(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::ContactId const& ourContactId) override;

			virtual bool isConnected() const;
			virtual void sendMessageReceivedAcknowledgement(openmittsu::protocol::ContactId const& messageSender, openmittsu::protocol::MessageId const& messageId);

			friend class openmittsu::test::MockNetworkSentMessageAcceptor;
		signals:
			void readyToAcceptMessages();
		private slots:
			void onConnectToFinished(int errCode);
		private:
			NetworkSentMessageAcceptor() {} // For Mock-testing only

			std::weak_ptr<openmittsu::network::ProtocolClient> m_protocolClient;

			void send(openmittsu::messages::contact::PreliminaryContactMessage const& message);
			void send(openmittsu::messages::group::PreliminaryGroupMessage const& message);
		};

	}
}

#endif // OPENMITTSU_DATABASE_NETWORKSENTMESSAGEACCEPTOR_H_
