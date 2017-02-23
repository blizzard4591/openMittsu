#include "utility/OptionMaster.h"

#include "exceptions/InternalErrorException.h"

#include <QByteArray>

OptionMaster* OptionMaster::instance = new OptionMaster();

OptionMaster::OptionMaster() : QObject(nullptr), settings("openmittsu.de", "OpenMittsu") {
	// Register all the options
	registerOption(QStringLiteral("options/sendTypingNotification"), Options::BOOLEAN_SEND_TYPING_NOTIFICATION, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("options/sendReadNotification"), Options::BOOLEAN_SEND_READ_NOTIFICATION, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("options/forceForegroundOnMessageReceived"), Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("options/playSoundOnMessageReceived"), Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("options/reconnectOnConnectionLoss"), Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("options/updateFeatureLevel"), Options::BOOLEAN_UPDATE_FEATURE_LEVEL, true, QMetaType::Type::Bool);
	registerOption(QStringLiteral("contactsFile"), Options::FILEPATH_CONTACTS_DATABASE, "", QMetaType::Type::QString);
	registerOption(QStringLiteral("clientConfigurationFile"), Options::FILEPATH_CLIENT_CONFIGURATION, "", QMetaType::Type::QString);
	registerOption(QStringLiteral("clientMainWindowGeometry"), Options::BINARY_MAINWINDOW_GEOMETRY, QByteArray(), QMetaType::Type::QByteArray);
	registerOption(QStringLiteral("clientMainWindowState"), Options::BINARY_MAINWINDOW_STATE, QByteArray(), QMetaType::Type::QByteArray);

	ensureOptionsExist();
}

OptionMaster::~OptionMaster() {
	//
}

OptionMaster* OptionMaster::getInstance() {
	return OptionMaster::instance;
}

bool OptionMaster::registerOption(QString const& optionName, Options const& option, QVariant const& defaultValue, QMetaType::Type optionType) {
	if (!optionToNameMap.contains(option)) {
		if (!defaultValue.canConvert(optionType)) {
			return false;
		}

		nameToOptionMap.insert(optionName, option);
		optionToNameMap.insert(option, optionName);
		optionToDefaultValueMap.insert(option, defaultValue);
		optionToTypeMap.insert(option, optionType);

		return true;
	} else {
		return false;
	}
}

QString OptionMaster::getOptionKeyForOption(Options const& option) const {
	if (optionToNameMap.contains(option)) {
		return optionToNameMap.value(option);
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::Options Key!";
	}
}

QMetaType::Type OptionMaster::getOptionTypeForOption(Options const& option) const {
	if (optionToTypeMap.contains(option)) {
		return optionToTypeMap.value(option);
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::Options Key!";
	}
}

void OptionMaster::ensureOptionsExist() {
	QHashIterator<Options, QString> i(optionToNameMap);
	while (i.hasNext()) {
		i.next();
		if (!settings.contains(i.value())) {
			settings.setValue(i.value(), optionToDefaultValueMap.value(i.key()));
		}
	}
	settings.sync();
}

bool OptionMaster::getOptionAsBool(Options const& option) const {
	QString const optionName = getOptionKeyForOption(option);
	if (settings.contains(optionName)) {
		QVariant v = settings.value(optionName);
		if (v.canConvert(QMetaType::Type::Bool)) {
			return v.toBool();
		} else {
			throw InternalErrorException() << "Can not convert requested option to bool!";
		}
	} else {
		throw InternalErrorException() << "Requested option does not exist!";
	}
}

QString OptionMaster::getOptionAsQString(Options const& option) const {
	QString const optionName = getOptionKeyForOption(option);
	if (settings.contains(optionName)) {
		QVariant v = settings.value(optionName);
		if (v.canConvert(QMetaType::Type::QString)) {
			return v.toString();
		} else {
			throw InternalErrorException() << "Can not convert requested option to QString!";
		}
	} else {
		throw InternalErrorException() << "Requested option does not exist!";
	}
}

QByteArray OptionMaster::getOptionAsQByteArray(Options const& option) const {
	QString const optionName = getOptionKeyForOption(option);
	if (settings.contains(optionName)) {
		QVariant v = settings.value(optionName);
		if (v.canConvert(QMetaType::Type::QByteArray)) {
			return v.toByteArray();
		} else {
			throw InternalErrorException() << "Can not convert requested option to QByteArray!";
		}
	} else {
		throw InternalErrorException() << "Requested option does not exist!";
	}
}

void OptionMaster::setOption(Options const& option, QVariant const& value) {
	QString const optionName = getOptionKeyForOption(option);

	if (value.canConvert(optionToTypeMap.value(option))) {
		settings.setValue(optionName, value);
	} else {
		throw InternalErrorException() << "Can not convert requested option to bool!";
	}
}

uint qHash(OptionMaster::Options const& key, uint seed) {
	return qHash(static_cast<std::size_t>(key), seed);
}

