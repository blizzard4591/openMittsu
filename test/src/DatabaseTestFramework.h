#ifndef OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_
#define OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_

#include "gtest/gtest.h"

#include <QDir>
#include <QFile>
#include <QSet>
#include <QString>

#include <memory>

#include "database/SimpleDatabase.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
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

		db = std::make_shared<openmittsu::database::Database>(databaseFilename, selfContactId, selfKeyPair, QStringLiteral("AAAAAAAA"), tempMediaStorageLocation);

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
};

#endif // OPENMITTSU_TESTS_DATABASETESTFRAMEWORK_H_
