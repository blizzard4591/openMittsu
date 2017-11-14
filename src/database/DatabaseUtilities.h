#ifndef OPENMITTSU_DATABASE_DATABASEUTILITIES_H_
#define OPENMITTSU_DATABASE_DATABASEUTILITIES_H_

#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QVariant>

#include "src/dataproviders/messages/Message.h"

namespace openmittsu {
	namespace database {

		class Database;

		class DatabaseUtilities {
		public:
			static int countQuery(QSqlDatabase const& database, QString const& tableName, QVariantMap const& whereQueryPart = {});
			static void prepareSetFieldsUpdateQuery(QSqlQuery& query, QString const& queryString, QVariantMap const& fieldsAndValues);
		};

	}
}

#endif // OPENMITTSU_DATABASE_DATABASEUTILITIES_H_
