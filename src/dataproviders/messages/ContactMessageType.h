#ifndef OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGETYPE_H_
#define OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGETYPE_H_

#include <QString>

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			enum class ContactMessageType {
				TEXT, IMAGE, LOCATION, POLL, VIDEO, AUDIO, BALLOT, FILE
			};

			class ContactMessageTypeHelper {
			public:
				static QString toQString(ContactMessageType const& messageType);
				static std::string toString(ContactMessageType const& messageType);
				static ContactMessageType fromString(QString const& messageTypeString);
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTACTMESSAGETYPE_H_
