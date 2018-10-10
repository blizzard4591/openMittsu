#ifndef OPENMITTSU_DATABASE_TESTDATABASEWRAPPER_H_
#define OPENMITTSU_DATABASE_TESTDATABASEWRAPPER_H_

#include <memory>

#include "src/database/DatabaseWrapper.h"

namespace openmittsu {
	namespace database {

		class TestDatabaseWrapper : public DatabaseWrapper {
			Q_OBJECT
		public:
			TestDatabaseWrapper(DatabasePointerAuthority const* databasePointerAuthority);
			TestDatabaseWrapper(TestDatabaseWrapper const& other);
			virtual ~TestDatabaseWrapper();
		};

	}
}

#endif // OPENMITTSU_DATABASE_TESTDATABASEWRAPPER_H_
