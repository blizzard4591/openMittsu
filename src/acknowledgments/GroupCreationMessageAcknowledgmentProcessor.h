#ifndef OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "src/acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"

#include <QSet>

namespace openmittsu {
	namespace acknowledgments {

		class GroupCreationMessageAcknowledgmentProcessor : public GroupContentMessageAcknowledgmentProcessor {
		public:
			GroupCreationMessageAcknowledgmentProcessor(openmittsu::protocol::GroupId const& groupId, QDateTime const& timeoutTime, openmittsu::protocol::MessageId const& groupUniqueMessageId, bool informViaSignal);
			virtual ~GroupCreationMessageAcknowledgmentProcessor();
		private:
			bool const m_informViaSignal;

			virtual void sendResultIfDone(openmittsu::network::ProtocolClient* protocolClient) override;
		};

	}
}

#endif // OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_
