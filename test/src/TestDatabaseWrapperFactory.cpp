#include "TestDatabaseWrapperFactory.h"

#include "src/database/DatabasePointerAuthority.h"
#include "TestDatabaseWrapper.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace database {

		TestDatabaseWrapperFactory::TestDatabaseWrapperFactory() : DatabaseWrapperFactory(), m_databasePointerAuthority(nullptr) {
			//
		}

		TestDatabaseWrapperFactory::TestDatabaseWrapperFactory(DatabasePointerAuthority const* databasePointerAuthority) : DatabaseWrapperFactory(databasePointerAuthority), m_databasePointerAuthority(databasePointerAuthority) {
			//
		}

		TestDatabaseWrapperFactory::~TestDatabaseWrapperFactory() {
			//
		}

		DatabaseWrapper TestDatabaseWrapperFactory::getDatabaseWrapper() const {
			return TestDatabaseWrapper(m_databasePointerAuthority);
		}

	}
}
