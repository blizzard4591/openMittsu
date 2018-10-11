#ifndef OPENMITTSU_DATABASE_INTERNAL_DATABASEUTILITIES_H_
#define OPENMITTSU_DATABASE_INTERNAL_DATABASEUTILITIES_H_

#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QVariant>

#include "src/database/internal/InternalDatabaseInterface.h"
#include "src/dataproviders/messages/Message.h"

namespace openmittsu {
	namespace database {
		namespace internal {

			class DatabaseUtilities {
			public:
				static int countQuery(InternalDatabaseInterface const* database, QString const& tableName, QVariantMap const& whereQueryPart = {});
				static void prepareSetFieldsUpdateQuery(QSqlQuery& query, QString const& queryString, QVariantMap const& fieldsAndValues);
			};

		}
	}
}

#endif // OPENMITTSU_DATABASE_INTERNAL_DATABASEUTILITIES_H_
