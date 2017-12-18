#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_

#include <QObject>

#include "src/dataproviders/NetworkSentMessageAcceptor.h"
#include "src/protocol/GroupId.h"
#include "src/messages/contact/ReceiptMessageContent.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenter : QObject {
			Q_OBJECT
		public:
			virtual ~MessageCenter() {}
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
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTER_H_
