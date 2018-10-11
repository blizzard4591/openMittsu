#ifndef OPENMITTSU_DATABASE_TESTDATABASEWRAPPERFACTORY_H_
#define OPENMITTSU_DATABASE_TESTDATABASEWRAPPERFACTORY_H_

#include <memory>

#include "src/database/DatabaseWrapperFactory.h"

namespace openmittsu {
	namespace database {
		class DatabasePointerAuthority;

		class TestDatabaseWrapperFactory : public DatabaseWrapperFactory {
		public:
			TestDatabaseWrapperFactory();
			TestDatabaseWrapperFactory(DatabasePointerAuthority const* databasePointerAuthority);
			virtual ~TestDatabaseWrapperFactory();

			virtual DatabaseWrapper getDatabaseWrapper() const override;
		private:
			DatabasePointerAuthority const* m_databasePointerAuthority;
		};

	}
}

#endif // OPENMITTSU_DATABASE_TESTDATABASEWRAPPERFACTORY_H_
