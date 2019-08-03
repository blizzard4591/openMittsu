#include "gtest/gtest.h"

#include <QString>

#include <memory>

#include "src/dataproviders/SimpleMessageCenter.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/utility/MakeUnique.h"

#include "DatabaseTestFramework.h"
#include "test/src/MockNetworkSentMessageAcceptor.h"

#include "TestDatabaseWrapperFactory.h"

using ::testing::Return;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::SizeIs;

TEST_F(DatabaseTestFramework, MessageCenterTestUnknownGroup) {
	ASSERT_EQ(0, db->getGroupCount());
	ASSERT_EQ(1, db->getContactCount());

	openmittsu::protocol::ContactId contactIdC(QStringLiteral("CCCCCCCC"));
	openmittsu::crypto::KeyPair contactIdCKeyPair(openmittsu::crypto::KeyPair::randomKey());
	ASSERT_NO_THROW(db->storeNewContact(contactIdC, contactIdCKeyPair));
	ASSERT_TRUE(db->hasContact(contactIdC));
	ASSERT_EQ(2, db->getContactCount());

	openmittsu::protocol::ContactId contactIdD(QStringLiteral("DDDDDDDD"));
	openmittsu::crypto::KeyPair contactIdDKeyPair(openmittsu::crypto::KeyPair::randomKey());
	ASSERT_NO_THROW(db->storeNewContact(contactIdD, contactIdDKeyPair));
	ASSERT_TRUE(db->hasContact(contactIdD));
	ASSERT_EQ(3, db->getContactCount());

	openmittsu::protocol::GroupId groupIdA(contactIdC, 1);
	ASSERT_FALSE(db->hasGroup(groupIdA));

	std::shared_ptr<openmittsu::test::MockNetworkSentMessageAcceptor> networkSentMessageAcceptor = std::make_shared<openmittsu::test::MockNetworkSentMessageAcceptor>();

	ON_CALL(*networkSentMessageAcceptor, isConnected()).WillByDefault(Return(true));
//	EXPECT_CALL(*networkSentMessageAcceptor, isConnected()).Times(2);
	EXPECT_CALL(*networkSentMessageAcceptor, processSentGroupSyncRequest(groupIdA, AllOf(Contains(contactIdC), SizeIs(1)), _, _));

	openmittsu::database::DatabasePointerAuthority dpa;
	dpa.setDatabase(db);

	std::unique_ptr<openmittsu::dataproviders::SimpleMessageCenter> mc = std::make_unique<openmittsu::dataproviders::SimpleMessageCenter>(openmittsu::database::TestDatabaseWrapperFactory(&dpa));
	mc->setNetworkSentMessageAcceptor(networkSentMessageAcceptor);
	QCoreApplication::processEvents();

	openmittsu::protocol::MessageId const messageA = this->getFreeMessageId();
	mc->processReceivedGroupMessageText(groupIdA, contactIdD, messageA, openmittsu::protocol::MessageTime::fromDatabase(1234567), openmittsu::protocol::MessageTime::fromDatabase(12345678), "Test 1");

	openmittsu::protocol::MessageId const messageB = this->getFreeMessageId();
	mc->processReceivedGroupMessageText(groupIdA, contactIdC, messageB, openmittsu::protocol::MessageTime::fromDatabase(2234567), openmittsu::protocol::MessageTime::fromDatabase(22345678), "Test 2");

	ASSERT_FALSE(db->hasGroup(groupIdA));
	ASSERT_EQ(db->getGroupMessageCount(), 1);

	openmittsu::protocol::MessageId const messageC = this->getFreeMessageId();
	mc->processReceivedGroupCreation(groupIdA, contactIdC, messageC, openmittsu::protocol::MessageTime::fromDatabase(3234567), openmittsu::protocol::MessageTime::fromDatabase(32345678), { contactIdC, contactIdD, selfContactId });
	ASSERT_TRUE(db->hasGroup(groupIdA));
	ASSERT_EQ(db->getGroupMessageCount(), 4);

	ASSERT_TRUE(mc->sendText(groupIdA, "TEST 3"));
	ASSERT_EQ(db->getGroupMessageCount(), 5);
}


