#ifndef OPENMITTSU_OPTIONS_OPTIONSTORAGE_H_
#define OPENMITTSU_OPTIONS_OPTIONSTORAGE_H_

#include <QMetaType>

namespace openmittsu {
	namespace options {
		enum class OptionStorage {
			STORAGE_SIMPLE,
			STORAGE_DATABASE
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::options::OptionStorage)

#endif // OPENMITTSU_OPTIONS_OPTIONSTORAGE_H_
