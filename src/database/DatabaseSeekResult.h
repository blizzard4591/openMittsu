#ifndef OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_
#define OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_

#include <QString>

#include "src/protocol/MessageId.h"

namespace openmittsu {
	namespace database {

		struct DatabaseSeekResult {
			bool operationResult;
			openmittsu::protocol::MessageId m_messageId;
			QString m_uid;
			qint64 m_sortByValue;

			DatabaseSeekResult() : operationResult(false), m_messageId(0), m_uid(), m_sortByValue(0) {
				//
			}
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::database::DatabaseSeekResult)

#endif // OPENMITTSU_DATABASE_DATABASESEEKRESULT_H_
