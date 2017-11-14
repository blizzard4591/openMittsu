#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_

#include <QVector>
#include <QString>

#include <cstdint>

namespace openmittsu {
	namespace dataproviders {

		class MessageSource {
		public:
			virtual ~MessageSource() {}

			virtual QVector<QString> getLastMessageUuids(std::size_t n) = 0;
			virtual int getMessageCount() const = 0;
		};

	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGESOURCE_H_
