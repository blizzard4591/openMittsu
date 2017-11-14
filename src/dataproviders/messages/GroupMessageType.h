#ifndef OPENMITTSU_DATAPROVIDERS_GROUPMESSAGETYPE_H_
#define OPENMITTSU_DATAPROVIDERS_GROUPMESSAGETYPE_H_

#include <QString>

namespace openmittsu {
	namespace dataproviders {
		namespace messages {

			enum class GroupMessageType {
				TEXT, IMAGE, LOCATION, POLL, VIDEO, AUDIO, BALLOT, FILE, SYNC_REQUEST, SET_TITLE, SET_IMAGE, GROUP_CREATION, LEAVE
			};

			class GroupMessageTypeHelper {
			public:
				static QString toQString(GroupMessageType const& messageType);
				static std::string toString(GroupMessageType const& messageType);
				static GroupMessageType fromString(QString const& messageTypeString);
			};

		}
	}
}

#endif // OPENMITTSU_DATAPROVIDERS_GROUPMESSAGETYPE_H_
