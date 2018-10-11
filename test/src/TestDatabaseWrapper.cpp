#include "TestDatabaseWrapper.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#define OPENMITTSU_TESTING
#include "src/utility/Wrapping.h"

#include "src/database/DatabaseReadonlyContactMessage.h"
#include "src/database/DatabaseReadonlyGroupMessage.h"

#include <QMetaObject>
#include <QMetaMethod>

namespace openmittsu {
	namespace database {

		TestDatabaseWrapper::TestDatabaseWrapper(DatabasePointerAuthority const* databasePointerAuthority) : DatabaseWrapper(databasePointerAuthority) {
			m_connectionType = Qt::ConnectionType::DirectConnection;
		}

		TestDatabaseWrapper::TestDatabaseWrapper(TestDatabaseWrapper const& other) : DatabaseWrapper(other) {
			m_connectionType = Qt::ConnectionType::DirectConnection;
		}

		TestDatabaseWrapper::~TestDatabaseWrapper() {
			//
		}

	}
}
