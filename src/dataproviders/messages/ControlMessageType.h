#ifndef OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGETYPE_H_
#define OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGETYPE_H_

#include <QString>

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			enum class ControlMessageType {
				RECEIVED, READ, AGREE, DISAGREE, GROUP_SYNC_REQUEST
			};

			class ControlMessageTypeHelper {
			public:
				static QString toString(ControlMessageType const& messageType);
				static ControlMessageType fromString(QString const& messageTypeString);
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_CONTROLMESSAGETYPE_H_
