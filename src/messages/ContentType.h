#ifndef OPENMITTSU_MESSAGES_CONTENTTYPE_H_
#define OPENMITTSU_MESSAGES_CONTENTTYPE_H_

#include <QString>

namespace openmittsu {
	namespace messages {

		enum class ContentType {
			TYPE_UNKNOWN,
			TYPE_CONTACT_TEXT,
			TYPE_CONTACT_PICTURE,
			TYPE_RECEIPT,
			TYPE_TYPINGNOTIFICATION,
			TYPE_GROUP_CREATION,
			TYPE_GROUP_TITLE,
			TYPE_GROUP_MESSAGE,
			TYPE_GROUP_LEAVE
		};

		class ContentTypeHelper {
		public:
			static QString convertContentTypeToString(ContentType const& messageType);
		};

	}
}

#endif // OPENMITTSU_MESSAGES_CONTENTTYPE_H_
