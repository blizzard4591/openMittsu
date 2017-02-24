#include "utility/OptionMaster.h"

#include "exceptions/InternalErrorException.h"
#include "utility/Logging.h"
#include "utility/MakeUnique.h"

#include <QByteArray>
#include <QCoreApplication>

OptionMaster* OptionMaster::instance = new OptionMaster();

OptionMaster::OptionMaster() : QObject(nullptr), settings(nullptr) {
	QCoreApplication::setOrganizationName("openMittsu");
	QCoreApplication::setOrganizationDomain("openmittsu.de");
	QCoreApplication::setApplicationName("OpenMittsu");

	settings = std::make_unique<QSettings>();
}

OptionMaster::~OptionMaster() {
	//
}

void OptionMaster::registerOptions() {
	if (settings->status() != QSettings::NoError) {
		throw InternalErrorException() << "Could not open settings!";
	}

	// Register option groups
	groupToNameMap.clear();

	groupToNameMap.insert(OptionGroups::GROUP_PRIVACY, tr("Privacy"));
	groupToNameMap.insert(OptionGroups::GROUP_NOTIFICATIONS, tr("Notifications"));
	groupToNameMap.insert(OptionGroups::GROUP_GENERAL, tr("General"));
	groupToNameMap.insert(OptionGroups::GROUP_INTERNAL, "");

	// Register all the options
	nameToOptionMap.clear();
	optionToOptionContainerMap.clear();
	groupToOptionsMap.clear();

	registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_TYPING_NOTIFICATION, QStringLiteral("options/sendTypingNotification"), tr("Whether the \"user is typing\" notification should be sent."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_PRIVACY, Options::BOOLEAN_SEND_READ_NOTIFICATION, QStringLiteral("options/sendReadNotification"), tr("Whether the \"message seen\" notification should be sent."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/forceForegroundOnMessageReceived"), tr("Whether the app should be forced into the foreground when a new message is received."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_NOTIFICATIONS, Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED, QStringLiteral("options/playSoundOnMessageReceived"), tr("Whether a sound should be played when a new message is received."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_RECONNECT_ON_CONNECTION_LOSS, QStringLiteral("options/reconnectOnConnectionLoss"), tr("Whether a reconnect should automatically be tried on a connection loss."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_GENERAL, Options::BOOLEAN_UPDATE_FEATURE_LEVEL, QStringLiteral("options/updateFeatureLevel"), tr("Whether the supported feature level of the used identity should be updated if the stored feature level is lower than the one supported by this app."), true, OptionTypes::TYPE_BOOL);
	registerOption(OptionGroups::GROUP_GENERAL, Options::FILEPATH_CONTACTS_DATABASE, QStringLiteral("options/database/contactsFile"), tr("The file path where the contacts database is stored."), "", OptionTypes::TYPE_FILEPATH);
	registerOption(OptionGroups::GROUP_GENERAL, Options::FILEPATH_CLIENT_CONFIGURATION, QStringLiteral("options/database/clientConfigurationFile"), tr("The file path where the client configuration containing the ID and private key is stored."), "", OptionTypes::TYPE_FILEPATH);
	registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_GEOMETRY, QStringLiteral("options/internal/clientMainWindowGeometry"), "", QByteArray(), OptionTypes::TYPE_BINARY);
	registerOption(OptionGroups::GROUP_INTERNAL, Options::BINARY_MAINWINDOW_STATE, QStringLiteral("options/internal/clientMainWindowState"), "", QByteArray(), OptionTypes::TYPE_BINARY);

	ensureOptionsExist();
}

OptionMaster* OptionMaster::getInstance() {
	return OptionMaster::instance;
}

QMetaType::Type OptionMaster::optionTypeToMetaType(OptionTypes const& type) const {
	if (type == OptionTypes::TYPE_BINARY) {
		return QMetaType::Type::QByteArray;
	} else if (type == OptionTypes::TYPE_BOOL) {
		return QMetaType::Type::Bool;
	} else if (type == OptionTypes::TYPE_FILEPATH) {
		return QMetaType::Type::QString;
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::OptionTypes Key!";
	}
}

bool OptionMaster::registerOption(OptionGroups const& optionGroup, Options const& option, QString const& optionName, QString const& optionDescription, QVariant const& defaultValue, OptionTypes optionType) {
	if (!optionToOptionContainerMap.contains(option)) {
		if (!defaultValue.canConvert(optionTypeToMetaType(optionType))) {
			return false;
		}

		OptionContainer container(optionGroup, option, optionName, optionDescription, defaultValue, optionType);

		nameToOptionMap.insert(optionName, option);
		optionToOptionContainerMap.insert(option, container);
		groupToOptionsMap.insert(optionGroup, option);

		return true;
	} else {
		return false;
	}
}

QString OptionMaster::getOptionKeyForOption(Options const& option) const {
	if (optionToOptionContainerMap.contains(option)) {
		return optionToOptionContainerMap.value(option).name;
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::Options Key!";
	}
}

OptionMaster::OptionTypes OptionMaster::getOptionTypeForOption(Options const& option) const {
	if (optionToOptionContainerMap.contains(option)) {
		return optionToOptionContainerMap.value(option).type;
	} else {
		throw InternalErrorException() << "Unknown OptionMaster::Options Key!";
	}
}

void OptionMaster::ensureOptionsExist() {
	QHashIterator<Options, OptionContainer> i(optionToOptionContainerMap);
	while (i.hasNext()) {
		i.next();
		if (!settings->contains(i.value().name)) {
			settings->setValue(i.value().name, i.value().defaultValue);
		}
	}
	settings->sync();
}

bool OptionMaster::getOptionAsBool(Options const& option) const {
	QString const optionName = getOptionKeyForOption(option);
	if (settings->contains(optionName)) {
		QVariant v = settings->value(optionName);
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
	if (settings->contains(optionName)) {
		QVariant v = settings->value(optionName);
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
	if (settings->contains(optionName)) {
		QVariant v = settings->value(optionName);
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

	if (value.canConvert(optionTypeToMetaType(optionToOptionContainerMap.value(option).type))) {
		settings->setValue(optionName, value);
		settings->sync();
	} else {
		throw InternalErrorException() << "Can not convert specified value to the option type!";
	}
}

uint qHash(OptionMaster::Options const& key, uint seed) {
	return qHash(static_cast<std::size_t>(key), seed);
}

uint qHash(OptionMaster::OptionGroups const& key, uint seed) {
	return qHash(static_cast<std::size_t>(key), seed);
}


