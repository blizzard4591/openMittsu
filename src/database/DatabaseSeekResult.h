#ifndef OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_
#define OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_

#include <QString>
#include <QSqlQuery>
#include <QVariant>

#include "src/database/DatabaseWrapper.h"
#include "src/dataproviders/messages/ReadonlyMessage.h"

namespace openmittsu {
	namespace database {

		struct DatabaseSeekResult {
			bool operationResult;
			openmittsu::protocol::MessageId m_messageId;
			QString m_uid;
			qint64 m_sortByValue;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::database::DatabaseSeekResult);

#endif // OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_
