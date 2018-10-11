#ifndef OPENMITTSU_TEST_MOCKNETWORKSENTMESSAGEACCEPTOR_H_
#define OPENMITTSU_TEST_MOCKNETWORKSENTMESSAGEACCEPTOR_H_

#include "src/network/ProtocolClient.h"
#include "src/dataproviders/NetworkSentMessageAcceptor.h"

#include "gmock/gmock.h" // Brings in Google Mock.

namespace openmittsu {
	namespace test {
		class MockProtocolClient : public openmittsu::network::ProtocolClient {
			//
		};


		class MockNetworkSentMessageAcceptor : public openmittsu::dataproviders::NetworkSentMessageAcceptor {
			Q_OBJECT
		public:
			MockNetworkSentMessageAcceptor() : openmittsu::dataproviders::NetworkSentMessageAcceptor() {}

			MOCK_METHOD4(processSentGroupSyncRequest, void(openmittsu::protocol::GroupId const& group, QSet<openmittsu::protocol::ContactId> const& targetGroupMembers, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent));
			MOCK_CONST_METHOD0(isConnected, bool());
			MOCK_METHOD2(sendMessageReceivedAcknowledgement, void(openmittsu::protocol::ContactId const& messageSender, openmittsu::protocol::MessageId const& messageId));
		};

	}
}

#endif // OPENMITTSU_TEST_MOCKNETWORKSENTMESSAGEACCEPTOR_H_
