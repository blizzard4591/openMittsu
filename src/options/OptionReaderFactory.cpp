#include "src/options/OptionReaderFactory.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace options {

		OptionReaderFactory::OptionReaderFactory() : m_databaseWrapperFactory(nullptr) {
			//
		}

		OptionReaderFactory::OptionReaderFactory(openmittsu::database::DatabaseWrapperFactory const& databaseWrapperFactory) : m_databaseWrapperFactory(std::make_unique<openmittsu::database::DatabaseWrapperFactory>(databaseWrapperFactory)) {
			//
		}

		OptionReaderFactory::OptionReaderFactory(OptionReaderFactory const& other) : m_databaseWrapperFactory(std::make_unique<openmittsu::database::DatabaseWrapperFactory>(*other.m_databaseWrapperFactory)) {
			//
		}

		OptionReaderFactory::~OptionReaderFactory() {
			//
		}

		std::unique_ptr<OptionReader> OptionReaderFactory::getOptionReader() const {
			return std::make_unique<OptionReader>(m_databaseWrapperFactory->getDatabaseWrapper());
		}

	}
}
