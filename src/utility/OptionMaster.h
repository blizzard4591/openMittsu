#ifndef OPENMITTSU_UTILITY_OPTIONMASTER_H_
#define OPENMITTSU_UTILITY_OPTIONMASTER_H_

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QMultiHash>

#include <memory>

namespace openmittsu {
	namespace database {
		class Database;
	}

	namespace dialogs {
		class OptionsDialog;
	}
}

namespace openmittsu {
	namespace utility {

		class OptionMaster : public QObject {
			Q_OBJECT
		public:
			friend class openmittsu::dialogs::OptionsDialog;

			OptionMaster();
			virtual ~OptionMaster();

			enum class OptionGroups {
				GROUP_PRIVACY,
				GROUP_NOTIFICATIONS,
				GROUP_GENERAL,
				GROUP_INTERNAL
			};

			enum class OptionTypes {
				TYPE_BOOL,
				TYPE_FILEPATH,
				TYPE_BINARY
			};

			enum class Options {
				BOOLEAN_SEND_TYPING_NOTIFICATION,
				BOOLEAN_SEND_READ_NOTIFICATION,
				BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED,
				BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED,
				BOOLEAN_RECONNECT_ON_CONNECTION_LOSS,
				BOOLEAN_UPDATE_FEATURE_LEVEL,
				BOOLEAN_TRUST_OTHERS,
				FILEPATH_DATABASE,
				FILEPATH_LEGACY_CLIENT_CONFIGURATION,
				FILEPATH_LEGACY_CONTACTS_DATABASE,
				BINARY_MAINWINDOW_GEOMETRY,
				BINARY_MAINWINDOW_STATE
			};

			enum class OptionStorage {
				STORAGE_SIMPLE,
				STORAGE_DATABASE
			};

			void setDatabase(std::shared_ptr<openmittsu::database::Database> const& database);

			bool getOptionAsBool(Options const& option) const;
			QString getOptionAsQString(Options const& option) const;
			QByteArray getOptionAsQByteArray(Options const& option) const;

			void setOption(Options const& option, QVariant const& value);

			void registerOptions();
		private:
			void ensureOptionsExist();
			QString getOptionKeyForOption(Options const& option) const;
			OptionTypes getOptionTypeForOption(Options const& option) const;

			QMetaType::Type optionTypeToMetaType(OptionTypes const& type) const;

			bool registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes const& optionType, OptionStorage const& optionStorage);

			std::unique_ptr<QSettings> m_settings;
			std::shared_ptr<openmittsu::database::Database> m_database;

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

			QHash<QString, Options> m_nameToOptionMap;
			QHash<Options, OptionContainer> m_optionToOptionContainerMap;
			QMultiHash<OptionGroups, Options> m_groupToOptionsMap;
			QHash<OptionGroups, QString> m_groupToNameMap;
		};

		uint qHash(OptionMaster::Options const& key, uint seed);
		uint qHash(OptionMaster::OptionGroups const& key, uint seed);

	}
}

Q_DECLARE_METATYPE(openmittsu::utility::OptionMaster::Options)
Q_DECLARE_METATYPE(openmittsu::utility::OptionMaster::OptionGroups)

#endif // OPENMITTSU_UTILITY_OPTIONMASTER_H_