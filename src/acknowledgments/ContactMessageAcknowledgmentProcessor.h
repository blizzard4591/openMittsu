#ifndef OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "src/acknowledgments/AcknowledgmentProcessor.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"

namespace openmittsu {
	namespace acknowledgments {

		class ContactMessageAcknowledgmentProcessor : public AcknowledgmentProcessor {
		public:
			ContactMessageAcknowledgmentProcessor(openmittsu::protocol::ContactId const& receiver, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& messageId);
			virtual ~ContactMessageAcknowledgmentProcessor();

			virtual bool isDone() const override;

			virtual openmittsu::protocol::ContactId const& getReceiver() const;
			virtual openmittsu::protocol::MessageId const& getMessageId() const;

			virtual void sendFailedTimeout(openmittsu::network::ProtocolClient* protocolClient) override;
			virtual void sendFailed(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) override;
			virtual void sendSuccess(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) override;

			virtual void addMessage(openmittsu::protocol::MessageId const& addedMessageId) override;
		private:
			openmittsu::protocol::ContactId const m_receiver;
			openmittsu::protocol::MessageId const m_messageId;
			bool m_receivedAck;
		};

	}
}

#endif // OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
