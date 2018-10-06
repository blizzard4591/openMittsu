#ifndef OPENMITTSU_OPTIONS_OPTIONREADERFACTORY_H_
#define OPENMITTSU_OPTIONS_OPTIONREADERFACTORY_H_

#include <memory>

#include "src/database/DatabaseWrapperFactory.h"
#include "src/options/OptionReader.h"

namespace openmittsu {
	namespace options {
		class OptionReaderFactory {
		public:
			OptionReaderFactory();
			OptionReaderFactory(openmittsu::database::DatabaseWrapperFactory const& databaseWrapperFactory);
			OptionReaderFactory(OptionReaderFactory const& other);
			virtual ~OptionReaderFactory();

			std::unique_ptr<OptionReader> getOptionReader() const;
		private:
			std::unique_ptr<openmittsu::database::DatabaseWrapperFactory> m_databaseWrapperFactory;
		};

	}
}

Q_DECLARE_METATYPE(openmittsu::options::OptionReaderFactory)

#endif // OPENMITTSU_OPTIONS_OPTIONREADERFACTORY_H_
