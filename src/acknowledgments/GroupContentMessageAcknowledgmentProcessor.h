#ifndef OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "src/acknowledgments/AcknowledgmentProcessor.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"

#include <QMutex>

namespace openmittsu {
	namespace acknowledgments {

		class GroupContentMessageAcknowledgmentProcessor : public AcknowledgmentProcessor {
		public:
			GroupContentMessageAcknowledgmentProcessor(openmittsu::protocol::GroupId const& groupId, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& groupUniqueMessageId);
			virtual ~GroupContentMessageAcknowledgmentProcessor();

			virtual bool isDone() const override;

			virtual openmittsu::protocol::GroupId const& getGroupId() const;
			virtual openmittsu::protocol::MessageId const& getGroupUniqueMessageId() const;

			virtual void addMessage(openmittsu::protocol::MessageId const& addedMessageId) override;

			virtual void sendFailedTimeout(openmittsu::network::ProtocolClient* protocolClient) override;
			virtual void sendFailed(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) override;
			virtual void sendSuccess(openmittsu::network::ProtocolClient* protocolClient, openmittsu::protocol::MessageId const& messageId) override;
		protected:
			std::size_t m_messageCount;
			bool m_hasFailedMessage;
		private:
			openmittsu::protocol::GroupId const m_groupId;
			openmittsu::protocol::MessageId const m_messageId;
			QMutex m_mutex;

			virtual void sendResultIfDone(openmittsu::network::ProtocolClient* protocolClient);
		};

	}
}

#endif // OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
