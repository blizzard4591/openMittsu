#ifndef OPENMITTSU_OPTIONS_OPTIONMASTER_H_
#define OPENMITTSU_OPTIONS_OPTIONMASTER_H_

#include "src/options/OptionReader.h"

namespace openmittsu {
	namespace dialogs {
		class OptionsDialog;
	}
}

namespace openmittsu {
	namespace options {

		class OptionMaster : public OptionReader {
			Q_OBJECT
		public:
			friend class openmittsu::dialogs::OptionsDialog;

			OptionMaster(openmittsu::database::DatabaseWrapper const& database);
			virtual ~OptionMaster();
			
			virtual void setOption(Options const& option, QVariant const& value);
		};

	}
}

#endif // OPENMITTSU_OPTIONS_OPTIONMASTER_H_