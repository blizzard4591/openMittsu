#ifndef OPENMITTSU_OPTIONS_OPTIONTYPES_H_
#define OPENMITTSU_OPTIONS_OPTIONTYPES_H_

#include <QMetaType>

namespace openmittsu {
	namespace options {
		enum class OptionTypes {
			TYPE_BOOL,
			TYPE_FILEPATH,
			TYPE_BINARY
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::options::OptionTypes)

#endif // OPENMITTSU_OPTIONS_OPTIONTYPES_H_
