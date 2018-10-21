#ifndef OPENMITTSU_DATABASE_MEDIAFILETYPE_H_
#define OPENMITTSU_DATABASE_MEDIAFILETYPE_H_

#include <QString>
#include <string>

namespace openmittsu {
	namespace database {
		enum class MediaFileType {
			TYPE_STANDARD,
			TYPE_THUMBNAIL
		};

		class MediaFileTypeHelper {
		public:
			static int toInt(MediaFileType const& messageType);
			static QString toQString(MediaFileType const& messageType);
			static std::string toString(MediaFileType const& messageType);
			static MediaFileType fromString(QString const& messageTypeString);
			static MediaFileType fromInt(int messageTypeInt);
		};
	}
}

#endif // OPENMITTSU_DATABASE_MEDIAFILETYPE_H_
