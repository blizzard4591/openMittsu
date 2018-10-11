#include "src/database/internal/DatabaseUtilities.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

#include <QVariant>

namespace openmittsu {
	namespace database {
		namespace internal {

			int DatabaseUtilities::countQuery(InternalDatabaseInterface const* database, QString const& tableName, QVariantMap const& whereQueryPart) {
				QSqlQuery query(database->getQueryObject());
				QString whereString;
				whereString.reserve(512);
				auto it = whereQueryPart.constBegin();
				auto const end = whereQueryPart.constEnd();

				int keyIndex = 1;
				for (; it != end; ++it) {
					if (whereString.isEmpty()) {
						whereString = QStringLiteral(" WHERE ");
					} else {
						whereString.append(QStringLiteral(" AND "));
					}
					whereString.append(QStringLiteral("`%1` = :value%2").arg(it.key()).arg(keyIndex, 4, 10, QChar('0')));
					++keyIndex;
				}

				query.prepare(QStringLiteral("SELECT Count(*) AS `count` FROM `%1`%2").arg(tableName).arg(whereString));

				it = whereQueryPart.constBegin();
				keyIndex = 1;
				for (; it != end; ++it) {
					query.bindValue(QStringLiteral(":value%1").arg(keyIndex, 4, 10, QChar('0')), it.value());
					++keyIndex;
				}

				if (query.exec() && query.isSelect() && query.next()) {
					return query.value(QStringLiteral("count")).toInt();
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute count query for table '" << tableName.toStdString() << "'. Query error: " << query.lastError().text().toStdString();
				}
			}

			void DatabaseUtilities::prepareSetFieldsUpdateQuery(QSqlQuery& query, QString const& queryString, QVariantMap const& fieldsAndValues) {
				if (fieldsAndValues.size() > 0) {
					QString setString;
					setString.reserve(512);
					auto it = fieldsAndValues.constBegin();
					auto const end = fieldsAndValues.constEnd();

					int keyIndex = 1;
					for (; it != end; ++it) {
						if (!setString.isEmpty()) {
							setString.append(QStringLiteral(", "));
						}
						setString.append(QStringLiteral("`%1` = :value%2").arg(it.key()).arg(keyIndex, 4, 10, QChar('0')));
						++keyIndex;
					}

					query.prepare(queryString.arg(setString));

					it = fieldsAndValues.constBegin();
					keyIndex = 1;
					for (; it != end; ++it) {
						query.bindValue(QStringLiteral(":value%1").arg(keyIndex, 4, 10, QChar('0')), it.value());
						++keyIndex;
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Can not build setFields update query part with empty field/value map, this should never happen!";
				}
			}

		}
	}
}
