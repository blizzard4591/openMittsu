#include "src/database/internal/DatabaseUniqueMessageIdGenerator.h"

#include "src/database/internal/InternalDatabaseInterface.h"

namespace openmittsu {
	namespace database {
		namespace internal {

			DatabaseUniqueMessageIdGenerator::DatabaseUniqueMessageIdGenerator(InternalDatabaseInterface* database) : m_database(database) {
				//
			}

			DatabaseUniqueMessageIdGenerator::~DatabaseUniqueMessageIdGenerator() {
				// Intentionally left empty.
			}

			openmittsu::protocol::MessageId DatabaseUniqueMessageIdGenerator::getNextUniqueMessageId(openmittsu::protocol::ContactId const& receiver) {
				return m_database->getNextMessageId(receiver);
			}

			openmittsu::protocol::MessageId DatabaseUniqueMessageIdGenerator::getNextUniqueMessageId(openmittsu::protocol::GroupId const& receiver) {
				return m_database->getNextMessageId(receiver);
			}

		}
	}
}
