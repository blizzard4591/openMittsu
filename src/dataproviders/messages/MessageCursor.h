#ifndef OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGECURSOR_H_
#define OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGECURSOR_H_

#include <QString>
#include <QVector>

#include "src/protocol/MessageId.h"

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			class MessageCursor {
			public:
				virtual ~MessageCursor() {}

				virtual bool isValid() const = 0;
				virtual bool seekToFirst() = 0;
				virtual bool seekToLast() = 0;
				virtual bool seek(openmittsu::protocol::MessageId const& messageId) = 0;
				virtual bool seekByUuid(QString const& uuid) = 0;

				virtual bool next() = 0;
				virtual bool previous() = 0;

				virtual openmittsu::protocol::MessageId const& getMessageId() const = 0;
				virtual QString const& getMessageUuid() const = 0;
				virtual QVector<QString> getLastMessages(std::size_t n) const = 0;
				virtual void deleteMessage(bool doAnnounce) = 0;
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_MESSAGES_MESSAGECURSOR_H_
