#ifndef OPENMITTSU_OPTIONS_OPTIONCONTAINER_H_
#define OPENMITTSU_OPTIONS_OPTIONCONTAINER_H_

#include <QString>
#include <QVariant>

#include "src/options/OptionGroups.h"
#include "src/options/Options.h"
#include "src/options/OptionStorage.h"
#include "src/options/OptionTypes.h"

namespace openmittsu {
	namespace options {
		struct OptionContainer {
			Options option;
			QString name;
			QString description;
			QVariant defaultValue;
			OptionTypes type;
			OptionGroups group;
			OptionStorage storage;

			OptionContainer(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& optionDefaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage) : option(option), name(optionName), description(optionDescription), defaultValue(optionDefaultValue), type(optionType), group(optionGroup), storage(optionStorage) {}
			OptionContainer() = default;
		};
	}
}

#endif // OPENMITTSU_OPTIONS_OPTIONCONTAINER_H_
