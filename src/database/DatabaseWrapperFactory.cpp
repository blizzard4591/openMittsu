#include "src/database/DatabaseWrapperFactory.h"

#include "src/database/DatabasePointerAuthority.h"
#include "src/database/DatabaseWrapper.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace database {

		DatabaseWrapperFactory::DatabaseWrapperFactory(DatabasePointerAuthority const& databasePointerAuthority) : m_databasePointerAuthority(databasePointerAuthority) {
			//
		}

		DatabaseWrapperFactory::~DatabaseWrapperFactory() {
			//
		}

		DatabaseWrapper DatabaseWrapperFactory::getDatabaseWrapper() const {
			return DatabaseWrapper(m_databasePointerAuthority);
		}

	}
}
