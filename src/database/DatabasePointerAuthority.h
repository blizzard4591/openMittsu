#ifndef OPENMITTSU_DATABASE_DATABASEPOINTERAUTHORITY_H_
#define OPENMITTSU_DATABASE_DATABASEPOINTERAUTHORITY_H_

#include <memory>

#include "src/database/Database.h"
#include "src/database/DatabaseWrapperFactory.h"

#include <QMutex>

namespace openmittsu {
	namespace database {

		class DatabasePointerAuthority : public QObject {
			Q_OBJECT
		public:
			DatabasePointerAuthority();
			virtual ~DatabasePointerAuthority();

		public slots:
			void setDatabase(std::shared_ptr<Database> newDatabase);

			std::weak_ptr<Database> getDatabaseWeak() const;
			std::shared_ptr<Database> getDatabaseStrong() const;

			DatabaseWrapperFactory getDatabaseWrapperFactory() const;
		signals:
			void newDatabaseAvailable();
		private:
			std::shared_ptr<Database> m_database;
			mutable QMutex m_mutex;
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEPOINTERAUTHORITY_H_
