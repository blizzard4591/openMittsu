#ifndef OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
#define OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_

#include "src/protocol/UniqueMessageIdGenerator.h"

namespace openmittsu {
	namespace database {
		class SimpleDatabase;

		class DatabaseUniqueMessageIdGenerator : public openmittsu::protocol::UniqueMessageIdGenerator{
		public:
			DatabaseUniqueMessageIdGenerator(SimpleDatabase& database);
			virtual ~DatabaseUniqueMessageIdGenerator();

			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::ContactId const& receiver) override;
			virtual openmittsu::protocol::MessageId getNextUniqueMessageId(openmittsu::protocol::GroupId const& receiver) override;
		private:
			SimpleDatabase & m_database;
		};
	}
}

#endif // OPENMITTSU_DATABASE_DATABASEUNIQUEMESSAGEIDGENERATOR_H_
