#ifndef OPENMITTSU_UTILITY_OPTIONMASTER_H_
#define OPENMITTSU_UTILITY_OPTIONMASTER_H_

#include <QObject>
#include <QSettings>
#include <QVariant>

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
private:
	OptionMaster();
	virtual ~OptionMaster();

	void ensureOptionsExist();
	void ensureOptionExists(Options const& option, QVariant const& defaultValue);
	static QString getOptionKeyForOption(Options const& option);

	static OptionMaster* instance;
	QSettings settings;
};

#endif // OPENMITTSU_UTILITY_OPTIONMASTER_H_