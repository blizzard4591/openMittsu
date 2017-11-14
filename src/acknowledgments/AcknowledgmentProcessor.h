#ifndef OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include <QDateTime>

namespace openmittsu {
	namespace network {
		class ProtocolClient;
	}

	namespace acknowledgments {

		class AcknowledgmentProcessor {
		public:
			AcknowledgmentProcessor(QDateTime const& timeoutTime);
			virtual ~AcknowledgmentProcessor();

			virtual QDateTime const& getTimeoutTime() const;
			virtual bool isDone() const = 0;

			virtual void sendFailedTimeout(openmittsu::network::ProtocolClient* protocolClient) = 0;
			virtual void sendFailed(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) = 0;
			virtual void sendSuccess(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) = 0;

			virtual void addMessage(openmittsu::protocol::MessageId const& addedMessageId) = 0;
		protected:
			void contactMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient);
			void groupMessageSendFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient);
			void contactMessageSendSuccess(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient);
			void groupMessageSendSuccess(openmittsu::protocol::GroupId const& groupId, openmittsu::protocol::MessageId const& messageId, openmittsu::network::ProtocolClient* protocolClient);

			void emitGroupCreationFailed(openmittsu::protocol::GroupId const& groupId, openmittsu::network::ProtocolClient* procolClient);
			void emitGroupCreationSuccess(openmittsu::protocol::GroupId const& groupId, openmittsu::network::ProtocolClient* procolClient);
		private:
			QDateTime const m_timeoutTime;
		};

	}
}

#endif // OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_
