#ifndef OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_
#define OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_

#include <memory>

#include "src/database/Database.h"

namespace openmittsu {
	namespace database {
		class DatabasePointerAuthority;

		class DatabaseWrapperFactory {
		public:
			DatabaseWrapperFactory(DatabasePointerAuthority const& databasePointerAuthority);
			virtual ~DatabaseWrapperFactory();

			DatabaseWrapper getDatabaseWrapper() const;
		private:
			DatabasePointerAuthority const& m_databasePointerAuthority;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_
