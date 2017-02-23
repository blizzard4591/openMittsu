#ifndef OPENMITTSU_UTILITY_OPTIONMASTER_H_
#define OPENMITTSU_UTILITY_OPTIONMASTER_H_

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QByteArray>
#include <QHash>

class OptionMaster : public QObject {
	Q_OBJECT
public:
	static OptionMaster* getInstance();

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
private:
	OptionMaster();
	virtual ~OptionMaster();

	void ensureOptionsExist();
	QString getOptionKeyForOption(Options const& option) const;
	QMetaType::Type getOptionTypeForOption(Options const& option) const;

	bool registerOption(QString const& optionName, Options const& option, QVariant const& defaultValue, QMetaType::Type optionType);

	static OptionMaster* instance;
	QSettings settings;
	QHash<QString, Options> nameToOptionMap;
	QHash<Options, QString> optionToNameMap;
	QHash<Options, QVariant> optionToDefaultValueMap;
	QHash<Options, QMetaType::Type> optionToTypeMap;
};

uint qHash(OptionMaster::Options const& key, uint seed);

Q_DECLARE_METATYPE(OptionMaster::Options)

#endif // OPENMITTSU_UTILITY_OPTIONMASTER_H_