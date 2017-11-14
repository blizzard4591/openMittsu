#ifndef OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_

#include "src/protocol/UniqueMessageIdGenerator.h"

namespace openmittsu {
	namespace database {
		class Database;

		class DatabaseUniqueMessageIdGenerator : public openmittsu::protocol::UniqueMessageIdGenerator{
		public:
			DatabaseUniqueMessageIdGenerator(Database& database);
			virtual ~DatabaseUniqueMessageIdGenerator();

			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::ContactId const& receiver) override;
			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::GroupId const& receiver) override;
		private:
			Database& m_database;
		};
	}
}

#endif // OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
