#include "src/utility/OptionMaster.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/database/Database.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"

#include <QByteArray>
#include <QCoreApplication>

namespace openmittsu {
	namespace utility {

		OptionMaster::OptionMaster() : QObject(nullptr), m_settings(nullptr), m_database(nullptr) {
			QCoreApplication::setOrganizationName("openMittsu");
			QCoreApplication::setOrganizationDomain("openmittsu.de");
			QCoreApplication::setApplicationName("OpenMittsu");

			m_settings = std::make_unique<QSettings>();

			registerOptions();
		}

		OptionMaster::~OptionMaster() {
			//
		}

		void OptionMaster::setDatabase(std::shared_ptr<openmittsu::database::Database> const& database) {
			this->m_database = database;
			ensureOptionsExist();
		}

		void OptionMaster::registerOptions() {
			if (m_settings->status() != QSettings::NoError) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open settings!";
			}

			// Register option groups
			m_groupToNameMap.clear();

			m_groupToNameMap.insert(OptionGroups::GROUP_PRIVACY, tr("Privacy"));
			m_groupToNameMap.insert(OptionGroups::GROUP_NOTIFICATIONS, tr("Notifications"));
			m_groupToNameMap.insert(OptionGroups::GROUP_GENERAL, tr("General"));
			m_groupToNameMap.insert(OptionGroups::GROUP_INTERNAL, "");

			// Register all the options
			m_nameToOptionMap.clear();
			m_optionToOptionContainerMap.clear();
			m_groupToOptionsMap.clear();

			registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_TYPING_NOTIFICATION, QStringLiteral("options/sendTypingNotification"), tr("Whether the \"user is typing\" notification should be sent."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_READ_NOTIFICATION, QStringLiteral("options/sendReadNotification"), tr("Whether the \"message seen\" notification should be sent."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/forceForegroundOnMessageReceived"), tr("Whether the app should be forced into the foreground when a new message is received."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/playSoundOnMessageReceived"), tr("Whether a sound should be played when a new message is received."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS, QStringLiteral("options/reconnectOnConnectionLoss"), tr("Whether a reconnect should automatically be tried on a connection loss."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_TRUST_OTHERS, QStringLiteral("options/trustOthers"), tr("Whether to accept messages from users whos group membership has not (yet) been confirmed."), false, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_UPDATE_FEATURE_LEVEL, QStringLiteral("options/updateFeatureLevel"), tr("Whether the supported feature level of the used identity should be updated if the stored feature level is lower than the one supported by this app."), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			registerOption(OptionGroups::GROUP_GENERAL, Options::FILEPATH_DATABASE, QStringLiteral("options/database/databaseFile"), tr("The file path where the main database file is stored."), "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);
			registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_GEOMETRY, QStringLiteral("options/internal/clientMainWindowGeometry"), "", QByteArray(), OptionTypes::TYPE_BINARY, OptionStorage::STORAGE_SIMPLE);
			registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_STATE, QStringLiteral("options/internal/clientMainWindowState"), "", QByteArray(), OptionTypes::TYPE_BINARY, OptionStorage::STORAGE_SIMPLE);
			registerOption(OptionGroups::GROUP_INTERNAL, Options::FILEPATH_LEGACY_CONTACTS_DATABASE, QStringLiteral("options/database/contactsFile"), "", "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);
			registerOption(OptionGroups::GROUP_INTERNAL, Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION, QStringLiteral("options/database/clientConfigurationFile"), "", "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);

			ensureOptionsExist();
		}

		QMetaType::Type OptionMaster::optionTypeToMetaType(OptionTypes const& type) const {
			if (type == OptionTypes::TYPE_BINARY) {
				return QMetaType::Type::QByteArray;
			} else if (type == OptionTypes::TYPE_BOOL) {
				return QMetaType::Type::Bool;
			} else if (type == OptionTypes::TYPE_FILEPATH) {
				return QMetaType::Type::QString;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionTypes Key with value " << static_cast<int>(type) << "!";
			}
		}

		bool OptionMaster::registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage) {
			if (!m_optionToOptionContainerMap.contains(option)) {
				if (!defaultValue.canConvert(optionTypeToMetaType(optionType))) {
					return false;
				}

				OptionContainer container(optionGroup, option, optionName, optionDescription, defaultValue, optionType, optionStorage);

				m_nameToOptionMap.insert(optionName, option);
				m_optionToOptionContainerMap.insert(option, container);
				m_groupToOptionsMap.insert(optionGroup, option);

				return true;
			} else {
				return false;
			}
		}

		QString OptionMaster::getOptionKeyForOption(Options const& option) const {
			if (m_optionToOptionContainerMap.contains(option)) {
				return m_optionToOptionContainerMap.value(option).name;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::Options Key with value " << static_cast<int>(option) << "!";
			}
		}

		OptionMaster::OptionTypes OptionMaster::getOptionTypeForOption(Options const& option) const {
			if (m_optionToOptionContainerMap.contains(option)) {
				return m_optionToOptionContainerMap.value(option).type;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::Options Key with value " << static_cast<int>(option) << "!";
			}
		}

		void OptionMaster::ensureOptionsExist() {
			QHashIterator<Options, OptionContainer> i(m_optionToOptionContainerMap);
			while (i.hasNext()) {
				i.next();
				if (i.value().storage == OptionStorage::STORAGE_DATABASE) {
					if (m_database == nullptr) {
						continue;
					} else if (!m_database->hasOption(i.value().name)) {
						if (i.value().type == OptionTypes::TYPE_BINARY) {
							m_database->setOptionValue(i.value().name, i.value().defaultValue.toByteArray());
						} else if (i.value().type == OptionTypes::TYPE_BOOL) {
							m_database->setOptionValue(i.value().name, i.value().defaultValue.toBool());
						} else if (i.value().type == OptionTypes::TYPE_FILEPATH) {
							m_database->setOptionValue(i.value().name, i.value().defaultValue.toString());
						} else {
							throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionTypes Key with value " << static_cast<int>(i.value().type) << " found on Option " << static_cast<int>(i.key()) << " with name \"" << i.value().name.toStdString() << "\"!";
						}
					}
				} else if (i.value().storage == OptionStorage::STORAGE_SIMPLE) {
					if (!m_settings->contains(i.value().name)) {
						if (i.value().type == OptionTypes::TYPE_BINARY) {
							m_settings->setValue(i.value().name, i.value().defaultValue.toByteArray());
						} else if (i.value().type == OptionTypes::TYPE_BOOL) {
							m_settings->setValue(i.value().name, i.value().defaultValue.toBool());
						} else if (i.value().type == OptionTypes::TYPE_FILEPATH) {
							m_settings->setValue(i.value().name, i.value().defaultValue.toString());
						} else {
							throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionTypes Key with value " << static_cast<int>(i.value().type) << " found on Option " << static_cast<int>(i.key()) << " with name \"" << i.value().name.toStdString() << "\"!";
						}
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(i.value().storage) << " found on Option " << static_cast<int>(i.key()) << " with name \"" << i.value().name.toStdString() << "\"!";
				}
			}
			m_settings->sync();
		}

		bool OptionMaster::getOptionAsBool(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (m_database == nullptr) {
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toBool();
				}

				if (m_database->hasOption(optionName)) {
					return m_database->getOptionValueAsBool(optionName);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				if (m_settings->contains(optionName)) {
					QVariant const v = m_settings->value(optionName);
					if (v.canConvert(QMetaType::Type::Bool)) {
						return v.toBool();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to bool!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

		QString OptionMaster::getOptionAsQString(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (m_database == nullptr) {
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toString();
				}

				if (m_database->hasOption(optionName)) {
					return m_database->getOptionValueAsString(optionName);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				if (m_settings->contains(optionName)) {
					QVariant const v = m_settings->value(optionName);
					if (v.canConvert(QMetaType::Type::QString)) {
						return v.toString();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to QString!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

		QByteArray OptionMaster::getOptionAsQByteArray(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (m_database == nullptr) {
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toByteArray();
				}

				if (m_database->hasOption(optionName)) {
					return m_database->getOptionValueAsByteArray(optionName);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				if (m_settings->contains(optionName)) {
					QVariant const v = m_settings->value(optionName);
					if (v.canConvert(QMetaType::Type::QByteArray)) {
						return v.toByteArray();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to QByteArray!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

		void OptionMaster::setOption(Options const& option, QVariant const& value) {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;
			OptionTypes const type = m_optionToOptionContainerMap.constFind(option)->type;


			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (m_database == nullptr) {
					return;
				}

				if (type == OptionTypes::TYPE_BINARY) {
					m_database->setOptionValue(optionName, value.toByteArray());
				} else if (type == OptionTypes::TYPE_BOOL) {
					m_database->setOptionValue(optionName, value.toBool());
				} else if (type == OptionTypes::TYPE_FILEPATH) {
					m_database->setOptionValue(optionName, value.toString());
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionTypes Key with value " << static_cast<int>(type) << "!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				if (value.canConvert(optionTypeToMetaType(type))) {
					m_settings->setValue(optionName, value);
					m_settings->sync();
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Can not convert specified value to the option type " << static_cast<int>(type) << " for option " << optionName.toStdString() << "!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionMaster::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}


		uint qHash(OptionMaster::Options const& key, uint seed) {
			return ::qHash(static_cast<std::size_t>(key), seed);
		}

		uint qHash(OptionMaster::OptionGroups const& key, uint seed) {
			return ::qHash(static_cast<std::size_t>(key), seed);
		}

	}
}
