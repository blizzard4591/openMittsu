#include "utility/OptionMaster.h"

#include "exceptions/InternalErrorException.h"

#include <QByteArray>

OptionMaster* OptionMaster::instance = new OptionMaster();

OptionMaster::OptionMaster() : QObject(nullptr), settings("openmittsu.de", "OpenMittsu") {
	//
}

OptionMaster::~OptionMaster() {
	//
}

OptionMaster* OptionMaster::getInstance() {
	return OptionMaster::instance;
}

QString OptionMaster::getOptionKeyForOption(Options const& option) {
	if (option == Options::BOOLEAN_SEND_TYPING_NOTIFICATION) {
		return QStringLiteral("options/sendTypingNotification");
	} else if (option == Options::BOOLEAN_SEND_READ_NOTIFICATION) {
		return QStringLiteral("options/sendReadNotification");
	} else if (option == Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED) {
		return QStringLiteral("options/forceForegroundOnMessageReceived");
	} else if (option == Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED) {
		return QStringLiteral("options/playSoundOnMessageReceived");
	} else if (option == Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS) {
		return QStringLiteral("options/reconnectOnConnectionLoss");
	} else if (option == Options::BOOLEAN_UPDATE_FEATURE_LEVEL) {
		return QStringLiteral("options/updateFeatureLevel");
	} else if (option == Options::FILEPATH_CONTACTS_DATABASE) {
		return QStringLiteral("contactsFile");
	} else if (option == Options::FILEPATH_CLIENT_CONFIGURATION) {
		return QStringLiteral("clientConfigurationFile");
	} else if (option == Options::BINARY_MAINWINDOW_GEOMETRY) {
		return QStringLiteral("clientMainWindowGeometry");
	} else if (option == Options::BINARY_MAINWINDOW_STATE) {
		return QStringLiteral("clientMainWindowState");
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::Options Key!";
	}
}

void OptionMaster::ensureOptionExists(Options const& option, QVariant const& defaultValue) {
	QString const optionName = getOptionKeyForOption(option);
	if (!settings.contains(optionName)) {
		settings.setValue(optionName, defaultValue);
	}
}

void OptionMaster::ensureOptionsExist() {
	ensureOptionExists(Options::BOOLEAN_SEND_TYPING_NOTIFICATION, true);
	ensureOptionExists(Options::BOOLEAN_SEND_READ_NOTIFICATION, true);
	ensureOptionExists(Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED, true);
	ensureOptionExists(Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED, true);
	ensureOptionExists(Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS, true);
	ensureOptionExists(Options::BOOLEAN_UPDATE_FEATURE_LEVEL, true);

	ensureOptionExists(Options::FILEPATH_CONTACTS_DATABASE, "");
	ensureOptionExists(Options::FILEPATH_CLIENT_CONFIGURATION, "");
	ensureOptionExists(Options::BINARY_MAINWINDOW_GEOMETRY, QByteArray());
	ensureOptionExists(Options::BINARY_MAINWINDOW_STATE, QByteArray());
}
