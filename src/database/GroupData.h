#ifndef OPENMITTSU_DATABASE_GROUPDATA_H_
#define OPENMITTSU_DATABASE_GROUPDATA_H_

#include "src/database/MediaFileItem.h"
#include "src/protocol/ContactId.h"

#include <QSet>
#include <QString>

namespace openmittsu {
	namespace database {
		struct GroupData {
			QString title;
			QString description;
			QSet<openmittsu::protocol::ContactId> members;
			bool hasImage;
			openmittsu::database::MediaFileItem image;
			bool isAwaitingSync;
			int messageCount;
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::database::GroupData)

#endif // OPENMITTSU_DATABASE_GROUPDATA_H_
