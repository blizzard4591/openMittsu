#include "DatabaseTestFramework.h"

#include "src/options/OptionMaster.h"
#include "TestDatabaseWrapper.h"
#include "TestDatabaseWrapperFactory.h"

TEST_F(DatabaseTestFramework, optionMaster) {
	/*
	bool getOptionAsBool(Options const& option) const;
	QString getOptionAsQString(Options const& option) const;
	QByteArray getOptionAsQByteArray(Options const& option) const;

	void setOption(Options const& option, QVariant const& value);
	*/
	
	/*
	
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
	
	*/

	openmittsu::database::DatabasePointerAuthority dpa;
	dpa.setDatabase(db);

	openmittsu::database::TestDatabaseWrapperFactory factory(&dpa);

	openmittsu::options::OptionMaster optionMaster(factory.getDatabaseWrapper());

	bool const valueBefore = optionMaster.getOptionAsBool(openmittsu::options::Options::BOOLEAN_SEND_TYPING_NOTIFICATION);
	optionMaster.setOption(openmittsu::options::Options::BOOLEAN_SEND_TYPING_NOTIFICATION, !valueBefore);
	bool const valueAfter = optionMaster.getOptionAsBool(openmittsu::options::Options::BOOLEAN_SEND_TYPING_NOTIFICATION);
	ASSERT_EQ(!valueBefore, valueAfter);
}
