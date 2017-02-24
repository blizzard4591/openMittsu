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

class OptionsDialog;

class OptionMaster : public QObject {
	Q_OBJECT
public:
	friend class OptionsDialog;
	static OptionMaster* getInstance();

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
		FILEPATH_CONTACTS_DATABASE,
		FILEPATH_CLIENT_CONFIGURATION,
		BINARY_MAINWINDOW_GEOMETRY,
		BINARY_MAINWINDOW_STATE
	};
	
	bool getOptionAsBool(Options const& option) const;
	QString getOptionAsQString(Options const& option) const;
	QByteArray getOptionAsQByteArray(Options const& option) const;

	void setOption(Options const& option, QVariant const& value);

	void registerOptions();
private:
	OptionMaster();
	virtual ~OptionMaster();

	void ensureOptionsExist();
	QString getOptionKeyForOption(Options const& option) const;
	OptionTypes getOptionTypeForOption(Options const& option) const;

	QMetaType::Type optionTypeToMetaType(OptionTypes const& type) const;

	bool registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes optionType);

	static OptionMaster* instance;
	std::unique_ptr<QSettings> settings;

	struct OptionContainer {
		Options option;
		QString name;
		QString description;
		QVariant defaultValue;
		OptionTypes type;
		OptionGroups group;

		OptionContainer(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& optionDefaultValue, OptionTypes optionType) : option(option), name(optionName), description(optionDescription), defaultValue(optionDefaultValue), type(optionType), group(optionGroup) {}
		OptionContainer() = default;
	};

	QHash<QString, Options> nameToOptionMap;
	QHash<Options, OptionContainer> optionToOptionContainerMap;
	QMultiHash<OptionGroups, Options> groupToOptionsMap;
	QHash<OptionGroups, QString> groupToNameMap;
};

uint qHash(OptionMaster::Options const& key, uint seed);
uint qHash(OptionMaster::OptionGroups const& key, uint seed);

Q_DECLARE_METATYPE(OptionMaster::Options)
Q_DECLARE_METATYPE(OptionMaster::OptionGroups)

#endif // OPENMITTSU_UTILITY_OPTIONMASTER_H_