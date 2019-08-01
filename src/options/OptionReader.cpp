#include "src/options/OptionReader.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/database/Database.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QByteArray>
#include <QCoreApplication>

namespace openmittsu {
	namespace options {

		OptionReader::OptionReader(openmittsu::database::DatabaseWrapper const& database) : QObject(nullptr), m_database(database) {
			QCoreApplication::setOrganizationName("openMittsu");
			QCoreApplication::setOrganizationDomain("openmittsu.de");
			QCoreApplication::setApplicationName("OpenMittsu");

			OPENMITTSU_CONNECT_QUEUED(&m_database, optionsChanged(), this, onDatabaseOptionsChanged());
			OPENMITTSU_CONNECT_QUEUED(&m_database, gotDatabase(), this, onDatabaseUpdated());

			registerOptions();

			onDatabaseOptionsChanged();
		}

		OptionReader::~OptionReader() {
			//
		}

		void OptionReader::forceInitialization() {
			onDatabaseUpdated();
		}

		void OptionReader::onDatabaseOptionsChanged() {
			m_databaseCache.clear();
			if (m_database.hasDatabase()) {
				m_databaseCache = m_database.getOptions();
			}
		}

		void OptionReader::onDatabaseUpdated() {
			m_databaseCache.clear();
			if (m_database.hasDatabase()) {
				m_databaseCache = m_database.getOptions();

				ensureOptionsExist();
			}
		}

		void OptionReader::registerOptions() {
			QSettings* settings = getSettings();
			if (settings->status() != QSettings::NoError) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open settings!";
			}

			// Clear data
			m_groupToNameMap.clear();
			m_nameToOptionMap.clear();
			m_optionToOptionContainerMap.clear();
			m_groupToOptionsMap.clear();

			registerOptions(this, m_groupToNameMap);

			ensureOptionsExist();
		}

		void OptionReader::registerOptions(OptionRegister* target, QHash<OptionGroups, QString>& groupsToName) {
			// Register option groups
			groupsToName.clear();

			groupsToName.insert(OptionGroups::GROUP_PRIVACY, tr("Privacy"));
			groupsToName.insert(OptionGroups::GROUP_NOTIFICATIONS, tr("Notifications"));
			groupsToName.insert(OptionGroups::GROUP_GENERAL, tr("General"));
			groupsToName.insert(OptionGroups::GROUP_INTERNAL, "");

			target->registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_TYPING_NOTIFICATION, QStringLiteral("options/sendTypingNotification"), tr("Send \"user is typing\" notifications"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_READ_NOTIFICATION, QStringLiteral("options/sendReadNotification"), tr("Send \"message seen\" notifications"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/forceForegroundOnMessageReceived"), tr("Force window into foreground on incoming message"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/playSoundOnMessageReceived"), tr("Play sound on incoming message"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS, QStringLiteral("options/reconnectOnConnectionLoss"), tr("Automatically attempt reconnect on a connection loss"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_TRUST_OTHERS, QStringLiteral("options/trustOthers"), tr("Accept messages from users whose group membership has not (yet) been confirmed"), false, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_UPDATE_FEATURE_LEVEL, QStringLiteral("options/updateFeatureLevel"), tr("Increase identity feature level to software feature level if possible"), true, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_GENERAL, Options::FILEPATH_DATABASE, QStringLiteral("options/database/databaseFile"), tr("Main database file path"), "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);
			target->registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_IGNORE_LEGACY_CONTACTS_DATABASE, QStringLiteral("options/ignoreLegacyContactsDatabase"), tr("Ignore still configured legacy contact database files (relevant when still using old versions in parallel)"), false, OptionTypes::TYPE_BOOL, OptionStorage::STORAGE_DATABASE);
			target->registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_GEOMETRY, QStringLiteral("options/internal/clientMainWindowGeometry"), "", QByteArray(), OptionTypes::TYPE_BINARY, OptionStorage::STORAGE_SIMPLE);
			target->registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_STATE, QStringLiteral("options/internal/clientMainWindowState"), "", QByteArray(), OptionTypes::TYPE_BINARY, OptionStorage::STORAGE_SIMPLE);
			target->registerOption(OptionGroups::GROUP_INTERNAL, Options::FILEPATH_LEGACY_CONTACTS_DATABASE, QStringLiteral("options/database/contactsFile"), "", "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);
			target->registerOption(OptionGroups::GROUP_INTERNAL, Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION, QStringLiteral("options/database/clientConfigurationFile"), "", "", OptionTypes::TYPE_FILEPATH, OptionStorage::STORAGE_SIMPLE);
		}

		QMetaType::Type OptionReader::optionTypeToMetaType(OptionTypes const& type) const {
			if (type == OptionTypes::TYPE_BINARY) {
				return QMetaType::Type::QByteArray;
			} else if (type == OptionTypes::TYPE_BOOL) {
				return QMetaType::Type::Bool;
			} else if (type == OptionTypes::TYPE_FILEPATH) {
				return QMetaType::Type::QString;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionTypes Key with value " << static_cast<int>(type) << "!";
			}
		}

		QSettings* OptionReader::getSettings() {
			static std::unique_ptr<QSettings> settings = std::make_unique<QSettings>();
			return settings.get();
		}

		bool OptionReader::registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage) {
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

		QString OptionReader::getOptionKeyForOption(Options const& option) const {
			if (m_optionToOptionContainerMap.contains(option)) {
				return m_optionToOptionContainerMap.value(option).name;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::Options Key with value " << static_cast<int>(option) << "!";
			}
		}

		OptionTypes OptionReader::getOptionTypeForOption(Options const& option) const {
			if (m_optionToOptionContainerMap.contains(option)) {
				return m_optionToOptionContainerMap.value(option).type;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::Options Key with value " << static_cast<int>(option) << "!";
			}
		}

		QString OptionReader::toStringRepresentation(QVariant const& value, OptionTypes const& optionType) {
			if (optionType == OptionTypes::TYPE_BINARY) {
				return QString(value.toByteArray().toHex());
			} else if (optionType == OptionTypes::TYPE_BOOL) {
				return ((value.toBool()) ? QStringLiteral("1") : QStringLiteral("0"));
			} else if (optionType == OptionTypes::TYPE_FILEPATH) {
				return value.toString();
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionTypes Key with value " << static_cast<int>(optionType) << " found!";
			}
		}

		void OptionReader::ensureOptionsExist() {
			QHash<QString, QString> optionsToSet;
			QSettings* settings = getSettings();

			QHashIterator<Options, OptionContainer> i(m_optionToOptionContainerMap);
			while (i.hasNext()) {
				i.next();
				if (i.value().storage == OptionStorage::STORAGE_DATABASE) {
					if (!m_database.hasDatabase()) {
						continue;
					} else if (!m_databaseCache.contains(i.value().name)) {
						optionsToSet.insert(i.value().name, toStringRepresentation(i.value().defaultValue, i.value().type));
					}
				} else if (i.value().storage == OptionStorage::STORAGE_SIMPLE) {
					if (!settings->contains(i.value().name)) {
						if (i.value().type == OptionTypes::TYPE_BINARY) {
							settings->setValue(i.value().name, i.value().defaultValue.toByteArray());
						} else if (i.value().type == OptionTypes::TYPE_BOOL) {
							settings->setValue(i.value().name, i.value().defaultValue.toBool());
						} else if (i.value().type == OptionTypes::TYPE_FILEPATH) {
							settings->setValue(i.value().name, i.value().defaultValue.toString());
						} else {
							throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionTypes Key with value " << static_cast<int>(i.value().type) << " found on Option " << static_cast<int>(i.key()) << " with name \"" << i.value().name.toStdString() << "\"!";
						}
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionStorage Key with value " << static_cast<int>(i.value().storage) << " found on Option " << static_cast<int>(i.key()) << " with name \"" << i.value().name.toStdString() << "\"!";
				}
			}
			settings->sync();

			if (!optionsToSet.empty()) {
				m_database.setOptions(optionsToSet);
			}
		}

		bool OptionReader::toBoolRepresentation(QString const& value) {
			if (value == QStringLiteral("1")) {
				return true;
			} else if (value == QStringLiteral("0")) {
				return false;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option to bool, database cache has value \"" << value.toStdString() << "\"!";
			}
		}

		bool OptionReader::getOptionAsBool(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (!m_database.hasDatabase()) {
					LOGGER_DEBUG("Returned default value for database option {} as database is not available.", optionName.toStdString());
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toBool();
				}

				if (m_databaseCache.contains(optionName)) {
					return toBoolRepresentation(m_databaseCache.value(optionName));
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				QSettings* settings = getSettings();
				if (settings->contains(optionName)) {
					QVariant const v = settings->value(optionName);
					if (v.canConvert(QMetaType::Type::Bool)) {
						return v.toBool();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to bool!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

		QString OptionReader::toQStringRepresentation(QString const& value) {
			return value;
		}

		QString OptionReader::getOptionAsQString(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (!m_database.hasDatabase()) {
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toString();
				}

				if (m_databaseCache.contains(optionName)) {
					return toQStringRepresentation(m_databaseCache.value(optionName));
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				QSettings* settings = getSettings();
				if (settings->contains(optionName)) {
					QVariant const v = settings->value(optionName);
					if (v.canConvert(QMetaType::Type::QString)) {
						return v.toString();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to QString!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}

		QByteArray OptionReader::toQByteArrayRepresentation(QString const& value) {
			QByteArray result = QByteArray::fromHex(value.toUtf8());
			return result;
		}

		QByteArray OptionReader::getOptionAsQByteArray(Options const& option) const {
			if (!m_optionToOptionContainerMap.contains(option)) {
				throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist!";
			}
			QString const optionName = getOptionKeyForOption(option);
			OptionStorage const optionStorage = m_optionToOptionContainerMap.constFind(option)->storage;

			if (optionStorage == OptionStorage::STORAGE_DATABASE) {
				if (!m_database.hasDatabase()) {
					return m_optionToOptionContainerMap.constFind(option)->defaultValue.toByteArray();
				}

				if (m_databaseCache.contains(optionName)) {
					return toQByteArrayRepresentation(m_databaseCache.value(optionName));
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in database!";
				}
			} else if (optionStorage == OptionStorage::STORAGE_SIMPLE) {
				QSettings* settings = getSettings();
				if (settings->contains(optionName)) {
					QVariant const v = settings->value(optionName);
					if (v.canConvert(QMetaType::Type::QByteArray)) {
						return v.toByteArray();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Can not convert requested option " << static_cast<int>(option) << " to QByteArray!";
					}
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Requested option " << static_cast<int>(option) << " does not exist in settings!";
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unknown OptionReader::OptionStorage Key with value " << static_cast<int>(optionStorage) << " found on Option " << static_cast<int>(option) << " with name \"" << optionName.toStdString() << "\"!";
			}
		}
	}
}
