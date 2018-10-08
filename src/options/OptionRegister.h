#ifndef OPENMITTSU_OPTIONS_OPTIONREGISTER_H_
#define OPENMITTSU_OPTIONS_OPTIONREGISTER_H_

#include <QString>
#include <QVariant>

#include "src/options/OptionGroups.h"
#include "src/options/Options.h"
#include "src/options/OptionStorage.h"
#include "src/options/OptionTypes.h"

namespace openmittsu {
	namespace options {
		class OptionReader;

		class OptionRegister {
		public:
			virtual ~OptionRegister() {}

			friend class OptionReader;
		protected:
			virtual bool registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage) = 0;
		};
	}
}

#endif // OPENMITTSU_OPTIONS_OPTIONREGISTER_H_
