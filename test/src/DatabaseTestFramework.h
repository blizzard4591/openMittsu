#ifndef OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_
#define OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_

#include "gtest/gtest.h"

#include <QDir>
#include <QFile>
#include <QHash>
#include <QSet>
#include <QString>

#include <memory>

#include "database/SimpleDatabase.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/MessageTime.h"
#include "crypto/KeyPair.h"

class DatabaseTestFramework : public ::testing::Test {
protected:
	static QString databaseFilename;
	static openmittsu::protocol::ContactId selfContactId;
	static openmittsu::crypto::KeyPair selfKeyPair;

	std::shared_ptr<openmittsu::database::SimpleDatabase> db;
	QSet<openmittsu::protocol::MessageId> usedMessageIds;
	QDir tempMediaStorageLocation;

	// Per-test-case set-up.
	// Called before the first test in this test case.
	// Can be omitted if not needed.
	static void SetUpTestCase() {
		//std::cout << "per-test-case setup" << std::endl;
	}

	// Per-test-case tear-down.
	// Called after the last test in this test case.
	// Can be omitted if not needed.
	static void TearDownTestCase() {
		//std::cout << "per-test-case teardown" << std::endl;
	}

	void addMessageId(openmittsu::protocol::MessageId const& usedMessageId) {
		usedMessageIds.insert(usedMessageId);
	}

	openmittsu::protocol::MessageId getFreeMessageId() {
		openmittsu::protocol::MessageId result(0);
		do {
			result = openmittsu::protocol::MessageId::random();
		} while (usedMessageIds.contains(result));
		usedMessageIds.insert(result);

		return result;
	}

	// You can define per-test set-up and tear-down logic as usual.
	virtual void SetUp() override {
		//std::cout << "per-test setup" << std::endl;
		db = nullptr;
		ensureFileDoesNotExist(databaseFilename);
		tempMediaStorageLocation = QDir::temp();
		tempMediaStorageLocation.mkdir(QStringLiteral("openMittsuTests-tmpdir"));
		ASSERT_TRUE(tempMediaStorageLocation.cd(QStringLiteral("openMittsuTests-tmpdir")));

		db = std::make_shared<openmittsu::database::SimpleDatabase>(databaseFilename, selfContactId, selfKeyPair, QStringLiteral("AAAAAAAA"), tempMediaStorageLocation);

		usedMessageIds.clear();
	}
	virtual void TearDown() override {
		//std::cout << "per-test teardown" << std::endl;
		db = nullptr;
		ensureFileDoesNotExist(databaseFilename);
		ASSERT_TRUE(tempMediaStorageLocation.removeRecursively());
	}

	void ensureFileDoesNotExist(QString const& filename) {
		if (QFile::exists(filename)) {
			ASSERT_TRUE(QFile::remove(filename));
		}
	}

	struct ContactMessageInfo {
		int messageTime;
		openmittsu::protocol::ContactId contact;
		ContactMessageInfo(int m, openmittsu::protocol::ContactId c) : messageTime(m), contact(c) {}
	};

	void setupContactsAndMessages(QHash<QString, ContactMessageInfo>& result, openmittsu::protocol::ContactId& usedContactIdA, openmittsu::protocol::ContactId& usedContactIdB, int messageCount) {
		openmittsu::protocol::ContactId contactIdB(QStringLiteral("BBBBBBBB"));
		openmittsu::crypto::KeyPair contactIdBKeyPair(openmittsu::crypto::KeyPair::randomKey());
		ASSERT_NO_THROW(db->storeNewContact(contactIdB, contactIdBKeyPair));
		ASSERT_TRUE(db->hasContact(contactIdB));

		openmittsu::protocol::ContactId contactIdC(QStringLiteral("CCCCCCCC"));
		openmittsu::crypto::KeyPair contactIdCKeyPair(openmittsu::crypto::KeyPair::randomKey());
		ASSERT_NO_THROW(db->storeNewContact(contactIdC, contactIdCKeyPair));
		ASSERT_TRUE(db->hasContact(contactIdC));

		openmittsu::database::internal::DatabaseContactMessageCursor cursorB = db->getMessageCursor(contactIdB);
		for (int i = 1; i <= messageCount; ++i) {
			openmittsu::protocol::MessageTime const messageTime(openmittsu::protocol::MessageTime::fromDatabase(i));
			openmittsu::protocol::MessageTime const messageTimeSent(openmittsu::protocol::MessageTime::fromDatabase(100 - i));
			if (i % 2 == 0) {
				openmittsu::protocol::MessageId messageId(0);
				ASSERT_NO_THROW(messageId = db->storeSentContactMessageText(contactIdB, messageTime, true, QStringLiteral("TestMessageA")));
				this->addMessageId(messageId);
				ASSERT_TRUE(cursorB.seek(messageId));
				result.insert(cursorB.getMessageUuid(), ContactMessageInfo(i, contactIdB));
			} else {
				openmittsu::protocol::MessageId mId = this->getFreeMessageId();
				ASSERT_NO_THROW(db->storeReceivedContactMessageText(contactIdB, mId, messageTimeSent, messageTime, QStringLiteral("TestMessageB")));
				ASSERT_TRUE(cursorB.seek(mId));
				result.insert(cursorB.getMessageUuid(), ContactMessageInfo(i, contactIdB));
			}
		}
		openmittsu::database::internal::DatabaseContactMessageCursor cursorC = db->getMessageCursor(contactIdC);
		for (int i = 1; i <= messageCount; ++i) {
			openmittsu::protocol::MessageTime const messageTime(openmittsu::protocol::MessageTime::fromDatabase(i));
			openmittsu::protocol::MessageTime const messageTimeSent(openmittsu::protocol::MessageTime::fromDatabase(100 - i));
			if (i % 2 == 0) {
				openmittsu::protocol::MessageId messageId(0);
				ASSERT_NO_THROW(messageId = db->storeSentContactMessageText(contactIdC, messageTime, true, QStringLiteral("TestMessageC")));
				this->addMessageId(messageId);
				ASSERT_TRUE(cursorC.seek(messageId));
				result.insert(cursorC.getMessageUuid(), ContactMessageInfo(i, contactIdC));
			} else {
				openmittsu::protocol::MessageId mId = this->getFreeMessageId();
				ASSERT_NO_THROW(db->storeReceivedContactMessageText(contactIdC, mId, messageTimeSent, messageTime, QStringLiteral("TestMessageD")));
				ASSERT_TRUE(cursorC.seek(mId));
				result.insert(cursorC.getMessageUuid(), ContactMessageInfo(i, contactIdC));
			}
		}
		usedContactIdA = contactIdB;
		usedContactIdB = contactIdC;
	}

	struct GroupMessageInfo {
		int messageTime;
		openmittsu::protocol::GroupId group;
		openmittsu::protocol::ContactId sender;
		GroupMessageInfo(int m, openmittsu::protocol::GroupId g, openmittsu::protocol::ContactId c) : messageTime(m), group(g), sender(c) {}
	};

	void setupGroupsAndMessages(QHash<QString, GroupMessageInfo>& result, openmittsu::protocol::GroupId& usedGroupIdA, openmittsu::protocol::GroupId& usedGroupIdB, int messageCount) {
		openmittsu::protocol::ContactId contactIdB(QStringLiteral("BBBBBBBB"));
		openmittsu::crypto::KeyPair contactIdBKeyPair(openmittsu::crypto::KeyPair::randomKey());
		ASSERT_NO_THROW(db->storeNewContact(contactIdB, contactIdBKeyPair));
		ASSERT_TRUE(db->hasContact(contactIdB));

		openmittsu::protocol::GroupId groupA(contactIdB, 1);
		ASSERT_NO_THROW(db->storeNewGroup(groupA, { contactIdB, selfContactId }, false));
		ASSERT_TRUE(db->hasGroup(groupA));

		openmittsu::protocol::GroupId groupB(contactIdB, 2);
		ASSERT_NO_THROW(db->storeNewGroup(groupB, { contactIdB, selfContactId }, false));
		ASSERT_TRUE(db->hasGroup(groupB));

		openmittsu::protocol::GroupId groupC(contactIdB, 3);
		ASSERT_NO_THROW(db->storeNewGroup(groupC, { contactIdB, selfContactId }, false));
		ASSERT_TRUE(db->hasGroup(groupC));

		openmittsu::database::internal::DatabaseGroupMessageCursor cursorA = db->getMessageCursor(groupA);
		for (int i = 1; i <= messageCount; ++i) {
			openmittsu::protocol::MessageTime const messageTime(openmittsu::protocol::MessageTime::fromDatabase(i));
			openmittsu::protocol::MessageTime const messageTimeSent(openmittsu::protocol::MessageTime::fromDatabase(100 - i));
			if (i % 2 == 0) {
				openmittsu::protocol::MessageId messageId(0);
				ASSERT_NO_THROW(messageId = db->storeSentGroupMessageText(groupA, messageTime, true, QStringLiteral("TestMessageA")));
				this->addMessageId(messageId);
				ASSERT_TRUE(cursorA.seek(messageId));
				result.insert(cursorA.getMessageUuid(), GroupMessageInfo(i, groupA, selfContactId));
			} else {
				openmittsu::protocol::MessageId mId = this->getFreeMessageId();
				ASSERT_NO_THROW(db->storeReceivedGroupMessageText(groupA, contactIdB, mId, messageTimeSent, messageTime, QStringLiteral("TestMessageB")));
				ASSERT_TRUE(cursorA.seek(mId));
				result.insert(cursorA.getMessageUuid(), GroupMessageInfo(i, groupA, contactIdB));
			}
		}
		openmittsu::database::internal::DatabaseGroupMessageCursor cursorB = db->getMessageCursor(groupB);
		for (int i = 1; i <= messageCount; ++i) {
			openmittsu::protocol::MessageTime const messageTime(openmittsu::protocol::MessageTime::fromDatabase(i));
			openmittsu::protocol::MessageTime const messageTimeSent(openmittsu::protocol::MessageTime::fromDatabase(100 - i));
			if (i % 2 == 0) {
				openmittsu::protocol::MessageId messageId(0);
				ASSERT_NO_THROW(messageId = db->storeSentGroupMessageText(groupB, messageTime, true, QStringLiteral("TestMessageC")));
				this->addMessageId(messageId);
				ASSERT_TRUE(cursorB.seek(messageId));
				result.insert(cursorB.getMessageUuid(), GroupMessageInfo(i, groupB, selfContactId));
			} else {
				openmittsu::protocol::MessageId mId = this->getFreeMessageId();
				ASSERT_NO_THROW(db->storeReceivedGroupMessageText(groupB, contactIdB, mId, messageTimeSent, messageTime, QStringLiteral("TestMessageD")));
				ASSERT_TRUE(cursorB.seek(mId));
				result.insert(cursorB.getMessageUuid(), GroupMessageInfo(i, groupB, contactIdB));
			}
		}
		usedGroupIdA = groupA;
		usedGroupIdB = groupB;
	}
};

#endif // OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_
