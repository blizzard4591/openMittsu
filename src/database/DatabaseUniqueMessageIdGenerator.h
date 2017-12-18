#ifndef OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_

#include "src/protocol/UniqueMessageIdGenerator.h"

namespace openmittsu {
	namespace database {
		class InternalDatabaseInterface;

		class DatabaseUniqueMessageIdGenerator : public openmittsu::protocol::UniqueMessageIdGenerator{
		public:
			DatabaseUniqueMessageIdGenerator(InternalDatabaseInterface* database);
			virtual ~DatabaseUniqueMessageIdGenerator();

			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::ContactId const& receiver) override;
			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::GroupId const& receiver) override;
		private:
			InternalDatabaseInterface* const m_database;
		};
	}
}

#endif // OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
