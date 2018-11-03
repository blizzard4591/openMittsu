#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_

#include <QVector>
#include <QString>

#include <cstdint>

#include "src/protocol/MessageTime.h"

namespace openmittsu {
	namespace dataproviders {

		class MessageSource {
		public:
			virtual ~MessageSource() {}

			virtual QVector<QString> getLastMessageUuids(std::size_t n) = 0;
			virtual int getMessageCount() const = 0;
			virtual void deleteMessageByUuid(QString const& uuid) = 0;
			virtual void deleteMessagesByAge(bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) = 0;
			virtual void deleteMessagesByCount(bool oldestOrNewest, int count) = 0;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_
