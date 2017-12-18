#include "src/database/DatabaseThreadWorker.h"

#include "src/database/SimpleDatabase.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace database {

		DatabaseThreadWorker::~DatabaseThreadWorker() {
			//
		}

		bool DatabaseThreadWorker::openDatabase(QString const& filename, QString const& password, QDir const& mediaStorageLocation) {
			std::shared_ptr<openmittsu::database::Database> newDatabase = std::make_shared<openmittsu::database::SimpleDatabase>(filename, password, mediaStorageLocation);
			if (newDatabase) {
				m_database = newDatabase;
				return true;
			} else {
				LOGGER()->warn("Failed to open database!");
				return false;
			}
		}

		bool DatabaseThreadWorker::createDatabase(QString const& filename, openmittsu::protocol::ContactId const& selfContact, openmittsu::crypto::KeyPair const& selfLongTermKeyPair, QString const& password, QDir const& mediaStorageLocation) {
			std::shared_ptr<openmittsu::database::Database> newDatabase = std::make_shared<openmittsu::database::SimpleDatabase>(filename, selfContact, selfLongTermKeyPair, password, mediaStorageLocation);
			if (newDatabase) {
				m_database = newDatabase;
				return true;
			} else {
				LOGGER()->warn("Failed to create database!");
				return false;
			}
		}

		bool DatabaseThreadWorker::hasDatabase() const {
			return m_database != nullptr;
		}

		std::shared_ptr<openmittsu::database::Database> DatabaseThreadWorker::getDatabase() {
			return m_database;
		}

		std::shared_ptr<openmittsu::database::Database> const& DatabaseThreadWorker::getDatabase() const {
			return m_database;
		}

	}
}
