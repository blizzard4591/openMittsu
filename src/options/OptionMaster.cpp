#include "src/options/OptionMaster.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/database/Database.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QByteArray>
#include <QCoreApplication>

namespace openmittsu {
	namespace options {

		OptionMaster::OptionMaster(openmittsu::database::DatabaseWrapper const& database) : OptionReader(database) {
			//
		}

		OptionMaster::~OptionMaster() {
			//
		}

		void OptionMaster::setOption(Options const& option, QVariant const& value) {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;
			OptionTypes const type = m_optionToOptionContainerMap.constFind(option)->type;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (!m_database.hasDatabase()) {
					return;
				}

				QHash<QString, QString> optionToSet;
				optionToSet.insert(optionName, toStringRepresentation(value, type));
				m_database.setOptions(optionToSet);

				if (m_database.hasDatabase()) {
					m_databaseCache = m_database.getOptions();
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				if (value.canConvert(optionTypeToMetaType(type))) {
					QSettings* settings = getSettings();
					settings->setValue(optionName, value);
					settings->sync();
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Can not convert specified value to the option type " << static_cast<int>(type) << " for option " << optionName.toStdString() << "!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

	}
}
