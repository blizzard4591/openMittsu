#include "src/database/DatabaseUniqueMessageIdGenerator.h"

#include "src/database/SimpleDatabase.h"

namespace openmittsu {
	namespace database {

		DatabaseUniqueMessageIdGenerator::DatabaseUniqueMessageIdGenerator(SimpleDatabase& database) : m_database(database) {
			//
		}

		DatabaseUniqueMessageIdGenerator::~DatabaseUniqueMessageIdGenerator() {
			// Intentionally left empty.
		}

		openmittsu::protocol::MessageId DatabaseUniqueMessageIdGenerator::getNextUniqueMessageId(openmittsu::protocol::ContactId const& receiver) {
			return m_database.getNextMessageId(receiver);
		}

		openmittsu::protocol::MessageId DatabaseUniqueMessageIdGenerator::getNextUniqueMessageId(openmittsu::protocol::GroupId const& receiver) {
			return m_database.getNextMessageId(receiver);
		}

	}
}
