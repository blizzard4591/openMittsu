#ifndef OPENMITTSU_OPTIONS_OPTIONDATA_H_
#define OPENMITTSU_OPTIONS_OPTIONDATA_H_

#include <QMetaType>
#include <QString>
#include <QVariant>

#include "src/options/OptionTypes.h"

namespace openmittsu {
	namespace options {
		struct OptionData {
			QString name;
			OptionTypes optionType;
			QVariant value;
		};
	}
}

Q_DECLARE_METATYPE(openmittsu::options::OptionData)

#endif // OPENMITTSU_OPTIONS_OPTIONDATA_H_
