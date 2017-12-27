#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_

#include "src/dataproviders/MessageCenter.h"

namespace openmittsu {
	namespace dataproviders {
		class MessageCenterWrapper : public MessageCenter {
		public:
			virtual ~MessageCenterWrapper() {}
		public:
			virtual bool sendText(openmittsu::protocol::ContactId const& receiver, QString const& text) override;
			virtual bool sendImage(openmittsu::protocol::ContactId const& receiver, QByteArray const& image, QString const& caption) override;
			virtual bool sendLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::utility::Location const& location) override;
			virtual bool sendReceipt(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) override;

			virtual void sendUserTypingStatus(openmittsu::protocol::ContactId const& receiver, bool isTyping) override;

			virtual bool sendText(openmittsu::protocol::GroupId const& group, QString const& text) override;
			virtual bool sendImage(openmittsu::protocol::GroupId const& group, QByteArray const& image, QString const& caption) override;
			virtual bool sendLocation(openmittsu::protocol::GroupId const& group, openmittsu::utility::Location const& location) override;
			virtual bool sendReceipt(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& receiptedMessageId, openmittsu::messages::contact::ReceiptMessageContent::ReceiptType const& receiptType) override;
			virtual bool sendLeave(openmittsu::protocol::GroupId const& group) override;
			virtual bool sendSyncRequest(openmittsu::protocol::GroupId const& group) override;

			virtual bool sendGroupCreation(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& members) override;
			virtual bool sendGroupTitle(openmittsu::protocol::GroupId const& group, QString const& title) override;
			virtual bool sendGroupImage(openmittsu::protocol::GroupId const& group, QByteArray const& image) override;

			virtual void setNetworkSentMessageAcceptor(std::shared_ptr<NetworkSentMessageAcceptor> const& newNetworkSentMessageAcceptor) override;
			virtual void setStorage(std::shared_ptr<openmittsu::database::Database> const& newStorage) override;
		};
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGECENTERWRAPPER_H_
