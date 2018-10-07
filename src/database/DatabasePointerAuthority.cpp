#include "src/database/DatabasePointerAuthority.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/database/DatabaseWrapperFactory.h"

#include <QMutexLocker>

namespace openmittsu {
	namespace database {

		DatabasePointerAuthority::DatabasePointerAuthority() : QObject() {
			//
		}

		DatabasePointerAuthority::~DatabasePointerAuthority() {
			//
		}

		void DatabasePointerAuthority::setDatabase(std::shared_ptr<Database> newDatabase) {
			{
				QMutexLocker lock(&m_mutex);
				m_database = newDatabase;
			}

			emit newDatabaseAvailable();
		}

		std::weak_ptr<Database> DatabasePointerAuthority::getDatabaseWeak() const {
			QMutexLocker lock(&m_mutex);
			if (!m_database) {
				LOGGER()->debug("The database in this authority was NULL, but a weak reference was requested!");
			}
			return m_database;
		}

		std::shared_ptr<Database> DatabasePointerAuthority::getDatabaseStrong() const {
			QMutexLocker lock(&m_mutex);
			if (!m_database) {
				LOGGER()->debug("The database in this authority was NULL, but a strong reference was requested!");
			}
			return m_database;
		}

		DatabaseWrapperFactory DatabasePointerAuthority::getDatabaseWrapperFactory() const {
			return DatabaseWrapperFactory(this);
		}

	}
}
