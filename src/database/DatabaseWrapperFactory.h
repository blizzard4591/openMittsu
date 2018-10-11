#ifndef OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_
#define OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_

#include <memory>

#include "src/database/Database.h"

namespace openmittsu {
	namespace database {
		class DatabasePointerAuthority;

		class DatabaseWrapperFactory {
		public:
			DatabaseWrapperFactory();
			DatabaseWrapperFactory(DatabasePointerAuthority const* databasePointerAuthority);
			virtual ~DatabaseWrapperFactory();

			virtual DatabaseWrapper getDatabaseWrapper() const;
		private:
			DatabasePointerAuthority const* m_databasePointerAuthority;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::database::DatabaseWrapperFactory)

#endif // OPENMITTSU_DATABASE_DATABASEWRAPPERFACTORY_H_
