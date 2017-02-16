#include <QtWidgets>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "Client.h"
#include "ServerConfiguration.h"
#include "ClientConfiguration.h"

#include "ui_main.h"

#include "wizards/BackupCreationWizard.h"
#include "wizards/GroupCreationWizard.h"
#include "wizards/LoadBackupWizard.h"
#include "dialogs/ShowIdentityAndPublicKeyDialog.h"
#include "dialogs/ContactAddDialog.h"
#include "dialogs/ContactEditDialog.h"
#include "dialogs/FingerprintDialog.h"
#include "dialogs/OptionsDialog.h"
#include "dialogs/UpdaterDialog.h"

#include "widgets/SimpleContactChatTab.h"
#include "widgets/SimpleGroupChatTab.h"
#include "widgets/ContactListWidgetItem.h"
#include "widgets/LicenseDialog.h"

#include "utility/Version.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"

#include "exceptions/InternalErrorException.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/ProtocolErrorException.h"

#include "tasks/IdentityReceiverCallbackTask.h"

#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/KeyRegistry.h"
#include "protocol/GroupRegistry.h"

#include "IdentityHelper.h"
#include "MessageCenter.h"
#include "Config.h"

Client::Client(QWidget *parent) : QMainWindow(parent), protocolClient(nullptr), settings(nullptr), audioOutput(nullptr), connectionState(ConnectionState::STATE_DISCONNECTED), serverConfiguration(nullptr), clientConfiguration(nullptr), contactRegistry(ContactRegistry::getInstance()) {
	ui.setupUi(this);
	ui.listContacts->setContextMenuPolicy(Qt::CustomContextMenu);
	connectionTimer.start(500);
	OPENMITTSU_CONNECT(&connectionTimer, timeout(), this, connectionTimerOnTimer());

	QCoreApplication::setOrganizationName("BliZZarD");
	QCoreApplication::setOrganizationDomain("philippberger.de");
	QCoreApplication::setApplicationName("OpenMittsu");

	this->settings = new QSettings(this);

	// Initialize in the right thread
	MessageCenter* mc = MessageCenter::getInstance();
	mc->setTabContainer(ui.tabWidget);

	OPENMITTSU_CONNECT(mc, newUnreadMessageAvailable(ChatTab*), this, messageCenterOnHasUnreadMessages(ChatTab*));

	QString const apiServerRootCertificate = QStringLiteral("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUVZVENDQTBtZ0F3SUJBZ0lKQU0xRFIvREJSRnBRTUEwR0NTcUdTSWIzRFFFQkJRVUFNSDB4Q3pBSkJnTlYKQkFZVEFrTklNUXN3Q1FZRFZRUUlFd0phU0RFUE1BMEdBMVVFQnhNR1duVnlhV05vTVJBd0RnWURWUVFLRXdkVQphSEpsWlcxaE1Rc3dDUVlEVlFRTEV3SkRRVEVUTUJFR0ExVUVBeE1LVkdoeVpXVnRZU0JEUVRFY01Cb0dDU3FHClNJYjNEUUVKQVJZTlkyRkFkR2h5WldWdFlTNWphREFlRncweE1qRXhNVE14TVRVNE5UaGFGdzB6TWpFeE1EZ3gKTVRVNE5UaGFNSDB4Q3pBSkJnTlZCQVlUQWtOSU1Rc3dDUVlEVlFRSUV3SmFTREVQTUEwR0ExVUVCeE1HV25WeQphV05vTVJBd0RnWURWUVFLRXdkVWFISmxaVzFoTVFzd0NRWURWUVFMRXdKRFFURVRNQkVHQTFVRUF4TUtWR2h5ClpXVnRZU0JEUVRFY01Cb0dDU3FHU0liM0RRRUpBUllOWTJGQWRHaHlaV1Z0WVM1amFEQ0NBU0l3RFFZSktvWkkKaHZjTkFRRUJCUUFEZ2dFUEFEQ0NBUW9DZ2dFQkFLOEdkb1Q3SXBOQzNEejdJVUdZVzlwT0J3eCs5RW5EWnJrTgpWRDhsM0tmQkhqR1RkaTlnUTZOaCttUTkveVE4MjU0VDJiaWc5cDBoY244a2pnRVFnSldIcE5oWW5PaHkzaTBqCmNtbHpiMU1GL2RlRmpKVnR1TVAzdHFUd2lNYXZwd2VvYTIwbEdEbi9DTFpvZHUwUmE4b0w3OGI2RlZ6dE5rV2cKUGRpV0NsTWswSlBQTWxmTEVpSzhoZkhFKzZtUlZYbWkxMml0SzFzZW1td3lIS2RqOWZHNFg5K3JRMnNLdUxmZQpqeDd1RnhuQUYrR2l2Q3VDbzh4Zk9lc0x3NzJ2eCtXN21tZFlzaGcvbFhPY3F2c3pRUS9MbUZFVlFZeE5hZWVWCm5QU0FzK2h0OHZVUFc0c1g5SWtYS1ZnQkpkMVIxaXNVcG9GNmRLbFVleG12THhFeWY1Y0NBd0VBQWFPQjR6Q0IKNERBZEJnTlZIUTRFRmdRVXc2TGFDNytKNjJyS2RhVEEzN2tBWVlVYnJrZ3dnYkFHQTFVZEl3U0JxRENCcFlBVQp3NkxhQzcrSjYycktkYVRBMzdrQVlZVWJya2loZ1lHa2Z6QjlNUXN3Q1FZRFZRUUdFd0pEU0RFTE1Ba0dBMVVFCkNCTUNXa2d4RHpBTkJnTlZCQWNUQmxwMWNtbGphREVRTUE0R0ExVUVDaE1IVkdoeVpXVnRZVEVMTUFrR0ExVUUKQ3hNQ1EwRXhFekFSQmdOVkJBTVRDbFJvY21WbGJXRWdRMEV4SERBYUJna3Foa2lHOXcwQkNRRVdEV05oUUhSbwpjbVZsYldFdVkyaUNDUUROUTBmd3dVUmFVREFNQmdOVkhSTUVCVEFEQVFIL01BMEdDU3FHU0liM0RRRUJCUVVBCkE0SUJBUUFSSE15SUhCREZ1bCtodmpBQ3Q2cjBFQUhZd1I5R1FTZ2hJUXNmSHQ4Y3lWY3ptRW5KSDlocnZoOVEKVml2bTdtcmZ2ZWlobU5YQW40V2xHd1ErQUN1VnRUTHh3OEVyYlNUN0lNQU94OW5wSGYva25nblo0blN3VVJGOQpyQ0V5SHExNzlwTlhwT3paMjU3RTVyMGF2TU5OWFhEd3VsdzAzaUJFMjFlYmQwMHBHMTFHVnEvSTI2cys4QmpuCkRLUlBxdUtyU080L2x1RUR2TDRuZ2lRalpwMzJTOVoxSzlzVk96cXRRN0k5enplVUFEbTNhVmEvQnBhdzRpTVIKMVNJN285YUpZaVJpMWd4WVAyQlVBMUlGcXI4Tnp5ZkdEN3RSSGRxN2JaT3hYQWx1djgxZGNiejBTQlg4U2dWMQo0SEVLYzZ4TUFObllzL2FZS2p2bVAwVnBPdlJVCi0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0=");
	PublicKey const longTermServerPublicKey = PublicKey::fromHexString(QStringLiteral("b851ae1bf275ebe6851ca7f5206b495080927159787e9aaabbeb4e55af09d805"));

	serverConfiguration = new ServerConfiguration(QStringLiteral("g-xx.0.threema.ch"), 5222, longTermServerPublicKey, QStringLiteral("https://api.threema.ch/identity/%1"), QStringLiteral("Threema/2.2A"), apiServerRootCertificate, QStringLiteral("https://%1.blob.threema.ch/%2"), QStringLiteral("https://%1.blob.threema.ch/%2/done"), QStringLiteral("https://upload.blob.threema.ch/upload"), QStringLiteral("Threema/2.2A"), apiServerRootCertificate);

	// Load stored settings
	if (settings->contains("clientConfigurationFile")) {
		if (validateClientConfigurationFile(settings->value("clientConfigurationFile").toString(), true)) {
			this->clientConfiguration = new ClientConfiguration(ClientConfiguration::fromFile(settings->value("clientConfigurationFile").toString()));
			updateClientSettingsInfo(settings->value("clientConfigurationFile").toString());
		} else {
			LOGGER_DEBUG("Removing key \"clientConfigurationFile\" from stored settings as the file is not valid.");
			settings->remove("clientConfigurationFile");
		}
	}
	// Load Contacts
	if (settings->contains("contactsFile")) {
		if (validateKnownIdentitiesFile(settings->value("contactsFile").toString(), true)) {
			contactRegistry->fromFile(settings->value("contactsFile").toString());
			updateKnownContactsInfo(settings->value("contactsFile").toString());
		} else {
			LOGGER_DEBUG("Removing key \"contactsFile\" from stored settings as the file is not valid.");
			settings->remove("contactsFile");
		}
	}

	OPENMITTSU_CONNECT(contactRegistry, identitiesChanged(), this, contactRegistryOnIdentitiesChanged());
	OPENMITTSU_CONNECT(ui.btnConnect, clicked(), this, btnConnectOnClick());
	OPENMITTSU_CONNECT(ui.btnOpenClientIni, clicked(), this, btnOpenClientIniOnClick());
	OPENMITTSU_CONNECT(ui.btnOpenContacts, clicked(), this, btnOpenContactsOnClick());
	OPENMITTSU_CONNECT(ui.listContacts, itemDoubleClicked(QListWidgetItem*), this, listContactsOnDoubleClick(QListWidgetItem*));
	OPENMITTSU_CONNECT(ui.listContacts, customContextMenuRequested(const QPoint&), this, listContactsOnContextMenu(const QPoint&));
	OPENMITTSU_CONNECT(ui.tabWidget, currentChanged(int), this, chatTabWidgetOnCurrentTabChanged(int));

	// Menus
	OPENMITTSU_CONNECT(ui.actionLicense, triggered(), this, menuAboutLicenseOnClick());
	OPENMITTSU_CONNECT(ui.actionAbout, triggered(), this, menuAboutAboutOnClick());
	OPENMITTSU_CONNECT(ui.actionAbout_Qt, triggered(), this, menuAboutAboutQtOnClick());
	OPENMITTSU_CONNECT(ui.actionAdd_a_Contact, triggered(), this, menuContactAddOnClick());
	OPENMITTSU_CONNECT(ui.actionDelete_a_Contact, triggered(), this, menuContactDeleteOnClick());
	OPENMITTSU_CONNECT(ui.actionEdit_a_Contact, triggered(), this, menuContactEditOnClick());
	OPENMITTSU_CONNECT(ui.actionSave_to_file, triggered(), this, menuContactSaveToFileOnClick());
	OPENMITTSU_CONNECT(ui.actionAdd_Group, triggered(), this, menuGroupAddOnClick());
	OPENMITTSU_CONNECT(ui.actionEdit_Group, triggered(), this, menuGroupEditOnClick());
	OPENMITTSU_CONNECT(ui.actionLeave_Group, triggered(), this, menuGroupLeaveOnClick());
	OPENMITTSU_CONNECT(ui.actionCreate_Backup, triggered(), this, menuIdentityCreateBackupOnClick());
	OPENMITTSU_CONNECT(ui.actionLoad_Backup, triggered(), this, menuIdentityLoadBackupOnClick());
	OPENMITTSU_CONNECT(ui.actionShow_Fingerprint, triggered(), this, menuIdentityShowFingerprintOnClick());
	OPENMITTSU_CONNECT(ui.actionShow_Public_Key, triggered(), this, menuIdentityShowPublicKeyOnClick());
	OPENMITTSU_CONNECT(ui.actionStatistics, triggered(), this, menuAboutStatisticsOnClick());
	OPENMITTSU_CONNECT(ui.actionOptions, triggered(), this, menuFileOptionsOnClick());
	OPENMITTSU_CONNECT(ui.actionExit, triggered(), this, menuFileExitOnClick());

	protocolClientThread.start();
	protocolClient = nullptr;
	OPENMITTSU_CONNECT(&protocolClientThread, finished(), this, threadFinished());

	// Load audio resources
	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);
	audioOutput = new QAudioOutput(format, this);

	OPENMITTSU_CONNECT(audioOutput, stateChanged(QAudio::State), this, audioOutputOnStateChanged(QAudio::State));

	receivedMessageAudioFile.setFileName(":/audio/ReceivedMessage.wav");
	if (!receivedMessageAudioFile.open(QFile::ReadOnly)) {
		throw InternalErrorException() << "Could not load audio file ReceivedMessage.wav in the Client.";
	}

#ifndef OPENMITTSU_CONFIG_DISABLE_VERSION_UPDATE_CHECK
	// Call Updater
	OPENMITTSU_CONNECT_QUEUED(&updater, foundNewVersion(int, int, int, int, QString, QString, QString), this, updaterFoundNewVersion(int, int, int, int, QString, QString, QString));
	QTimer::singleShot(0, &updater, SLOT(start()));
#endif

	// Call the setup() function in the thread
	QTimer::singleShot(0, protocolClient, SLOT(setup()));
	contactRegistryOnIdentitiesChanged();

	// Restore Window location and size
	restoreGeometry(settings->value("clientMainWindowGeometry").toByteArray());
	restoreState(settings->value("clientMainWindowState").toByteArray());
}

void Client::closeEvent(QCloseEvent* event) {
	// Save location and size of window
	settings->setValue("clientMainWindowGeometry", saveGeometry());
	settings->setValue("clientMainWindowState", saveState());
	
	QMainWindow::closeEvent(event);
}

void Client::setupProtocolClient() {
	if (protocolClient != nullptr) {
		MessageCenter::getInstance()->setProtocolClient(nullptr);
		if (protocolClient->getIsConnected()) {
			protocolClient->disconnectFromServer();
		}
		QMetaObject::invokeMethod(protocolClient, "teardown", Qt::QueuedConnection);
		protocolClient->deleteLater();
		protocolClient = nullptr;
	}

	QString const nickname = contactRegistry->getNickname(clientConfiguration->getClientIdentity());
	if (nickname.compare(QStringLiteral("You"), Qt::CaseInsensitive) == 0) {
		LOGGER()->info("Using only ID as PushFromID token (for iOS Push Receivers).");
		protocolClient = new ProtocolClient(KeyRegistry(clientConfiguration->getClientLongTermKeyPair(), serverConfiguration->getServerLongTermPublicKey(), contactRegistry->getKnownIdentitiesWithPublicKeys()), GroupRegistry(contactRegistry->getKnownGroupsWithMembersAndTitles()), MessageCenter::getInstance()->getUniqueMessgeIdGenerator(), *serverConfiguration, *clientConfiguration, MessageCenter::getInstance(), PushFromId(clientConfiguration->getClientIdentity()));
	} else {
		protocolClient = new ProtocolClient(KeyRegistry(clientConfiguration->getClientLongTermKeyPair(), serverConfiguration->getServerLongTermPublicKey(), contactRegistry->getKnownIdentitiesWithPublicKeys()), GroupRegistry(contactRegistry->getKnownGroupsWithMembersAndTitles()), MessageCenter::getInstance()->getUniqueMessgeIdGenerator(), *serverConfiguration, *clientConfiguration, MessageCenter::getInstance(), PushFromId(nickname));
		LOGGER()->info("Using nickname \"{}\" as PushFromID token (for iOS Push Receivers).", nickname.toStdString());
	}

	protocolClient->moveToThread(&protocolClientThread);

	OPENMITTSU_CONNECT(protocolClient, connectToFinished(int, QString), this, protocolClientOnConnectToFinished(int, QString));
	OPENMITTSU_CONNECT(protocolClient, readyConnect(), this, protocolClientOnReadyConnect());
	OPENMITTSU_CONNECT(protocolClient, lostConnection(), this, protocolClientOnLostConnection());
	OPENMITTSU_CONNECT(protocolClient, duplicateIdUsageDetected(), this, protocolClientOnDuplicateIdUsageDetected());

	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(protocolClient, setupDone(), &eventLoop, quit());

	QMetaObject::invokeMethod(protocolClient, "setup", Qt::QueuedConnection);
	eventLoop.exec(); // blocks until "finished()" has been called

	MessageCenter::getInstance()->setProtocolClient(protocolClient);
}

void Client::threadFinished() {
	LOGGER_DEBUG("Client::threadFinished - the worker thread finished.");
	if (protocolClient != nullptr) {
		QMetaObject::invokeMethod(protocolClient, "teardown", Qt::QueuedConnection);
		protocolClient->deleteLater();
		protocolClient = nullptr;
	}
	LOGGER()->critical("Since the worker thread just terminated, everything will close now.");
	this->close();
}

void Client::updaterFoundNewVersion(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString gitHash, QString channel, QString link) {
	UpdaterDialog updateDialog(versionMajor, versionMinor, versionPatch, commitsSinceTag, gitHash, channel, link, this);
	updateDialog.exec();
}

void Client::audioOutputOnStateChanged(QAudio::State state) {
	LOGGER_DEBUG("Audio state change.");
	switch (state) {
	case QAudio::IdleState:
		// Finished playing (no more data)
		audioOutput->stop();
		receivedMessageAudioFile.reset();
		break;

	case QAudio::StoppedState:
		// Stopped for other reasons
		receivedMessageAudioFile.reset();
		if (audioOutput->error() != QAudio::NoError) {
			LOGGER()->warn("Error while playing sound: {}", audioOutput->error());
		}
		break;

	default:
		break;
	}
}

void Client::updateClientSettingsInfo(QString const& currentFileName) {
	ui.lblClientIni->setText(currentFileName);
	if (clientConfiguration != nullptr) {
		if (!contactRegistry->hasIdentity(clientConfiguration->getClientIdentity())) {
			IdentityContact* iContact = new IdentityContact(clientConfiguration->getClientIdentity(), clientConfiguration->getClientLongTermKeyPair());
			iContact->setNickname("You");
			contactRegistry->addIdentity(iContact);
		}
		contactRegistry->setSelfContact(contactRegistry->getIdentity(clientConfiguration->getClientIdentity()));
	}
}

void Client::updateKnownContactsInfo(QString const& currentFileName) {
	ui.lblContacts->setText(currentFileName);
}

void Client::btnConnectOnClick() {
	if (connectionState == ConnectionState::STATE_DISCONNECTED) {
		if ((serverConfiguration == nullptr) || (clientConfiguration == nullptr)) {
			QMessageBox::warning(this, "Can not connect", "Please choose a valid client configuration file first.");
			return;
		}

		setupProtocolClient();

		ui.btnConnect->setEnabled(false);
		ui.btnConnect->setText("Connecting...");
		QTimer::singleShot(0, protocolClient, SLOT(connectToServer()));
	} else if (connectionState == ConnectionState::STATE_CONNECTING) {
		// No click should be possible in this state
		ui.btnConnect->setEnabled(false);
	} else if (connectionState == ConnectionState::STATE_CONNECTED) {
		ui.btnConnect->setEnabled(false);
		ui.btnConnect->setText("Disconnecting...");
		QTimer::singleShot(0, protocolClient, SLOT(disconnectFromServer()));
	}
}

bool Client::validateClientConfigurationFile(QString const& fileName, bool quiet) {
	try {
		QFile inputFile(fileName);
		if (!inputFile.exists()) {
			return false;
		}

		ClientConfiguration sc(ClientConfiguration::fromFile(fileName));
		return true;
	} catch (IllegalArgumentException& iax) {
		if (!quiet) {
			QMessageBox::warning(this, "Invalid Client Configuration file", QString("The selected file can not be used.\nReason: %1").arg(iax.what()));
		}
	}
	return false;
}

bool Client::validateKnownIdentitiesFile(QString const& fileName, bool quiet) {
	try {
		QFile inputFile(fileName);
		if (!inputFile.exists()) {
			return false;
		}

		contactRegistry->fromFile(fileName, true);
		return true;
	} catch (IllegalArgumentException& iax) {
		if (!quiet) {
			QMessageBox::warning(this, "Invalid Contacts file", QString("The selected file can not be used.\nReason: %1").arg(iax.what()));
		}
	}
	return false;
}

void Client::btnOpenClientIniOnClick() {
	QString fileName = QFileDialog::getOpenFileName(this, "Select a ClientConfiguration.ini file");
	if (fileName.isNull() || fileName.isEmpty() || !validateClientConfigurationFile(fileName)) {
		return;
	}

	if (clientConfiguration != nullptr) {
		delete clientConfiguration;
		clientConfiguration = nullptr;
	}

	clientConfiguration = new ClientConfiguration(ClientConfiguration::fromFile(fileName));
	settings->setValue("clientConfigurationFile", fileName);
	updateClientSettingsInfo(fileName);
}

void Client::btnOpenContactsOnClick() {
	QString fileName = QFileDialog::getOpenFileName(this, "Select a file containing known contacts");
	if (fileName.isNull() || fileName.isEmpty() || !validateKnownIdentitiesFile(fileName)) {
		return;
	}
	contactRegistry->fromFile(fileName);
	settings->setValue("contactsFile", fileName);
	updateKnownContactsInfo(fileName);
}

void Client::contactRegistryOnIdentitiesChanged() {
	LOGGER_DEBUG("Updating contacts list on IdentitiesChanged() signal.");
	ui.listContacts->clear();
	
	QList<ContactId> knownIdentities = contactRegistry->getIdentities();
	QList<ContactId>::const_iterator it = knownIdentities.constBegin();
	QList<ContactId>::const_iterator end = knownIdentities.constEnd();

	ContactId const selfIdentity = (clientConfiguration == nullptr) ? ContactId(0) : clientConfiguration->getClientIdentity();
	for (; it != end; ++it) {
		if (*it == selfIdentity) {
			continue;
		}
		ContactListWidgetItem* clwi = new ContactListWidgetItem(contactRegistry->getIdentity(*it), contactRegistry->getNickname(*it));
		ui.listContacts->addItem(clwi);
	}

	// Groups
	QList<GroupId> knownGroups = contactRegistry->getGroups();
	QList<GroupId>::const_iterator itGroups = knownGroups.constBegin();
	QList<GroupId>::const_iterator endGroups = knownGroups.constEnd();

	for (; itGroups != endGroups; ++itGroups) {
		ContactListWidgetItem* clwi = new ContactListWidgetItem(contactRegistry->getGroup(*itGroups), contactRegistry->getNickname(*itGroups));
		ui.listContacts->addItem(clwi);
	}

	// Save to file
	if (settings->contains("contactsFile")) {
		contactRegistry->toFile(settings->value("contactsFile").toString());
	}
}

void Client::messageCenterOnHasUnreadMessages(ChatTab* tab) {
	LOGGER_DEBUG("Activating window for unread messages...");
	this->activateWindow();
	// Do not restart audio, let it play.
	if (audioOutput->state() != QAudio::State::IdleState) {
		return;
	}

	// Play audio for incoming and unseen message.
	receivedMessageAudioFile.reset();
	audioOutput->start(&receivedMessageAudioFile);
}

void Client::protocolClientOnReadyConnect() {
	LOGGER_DEBUG("In Client: protocolClientOnReadyConnect()");
	connectionState = ConnectionState::STATE_DISCONNECTED;
	ui.btnConnect->setText("Connect");
	ui.btnConnect->setEnabled(true);
	uiFocusOnOverviewTab();
}

void Client::protocolClientOnLostConnection() {
	LOGGER_DEBUG("In Client: protocolClientOnLostConnection()");
	connectionState = ConnectionState::STATE_DISCONNECTED;
	ui.btnConnect->setText("Connect");
	ui.btnConnect->setEnabled(true);
	uiFocusOnOverviewTab();
}

void Client::protocolClientOnDuplicateIdUsageDetected() {
	LOGGER_DEBUG("In Client: protocolClientOnDuplicateIdUsageDetected()");
	QMessageBox::warning(this, "Duplicate ID Usage", "It seems that the ID you are using is also in use on a different device.\nEach ID can only be used on one endpoint at a time!\nTry using different IDs for each endpoints and building groups per contact containing all your IDs and those of the contact.\nUse the airplane mode on smart phones, if necessary.");
}

void Client::protocolClientOnConnectToFinished(int errCode, QString message) {
	LOGGER_DEBUG("In Client: protocolClientOnConnectToFinished({}, {})", errCode, message.toStdString());
	uiFocusOnOverviewTab();
	if (errCode == 0) {
		connectionState = ConnectionState::STATE_CONNECTED;
		ui.btnConnect->setText("Disconnect");
		ui.btnConnect->setEnabled(true);
	} else {
		connectionState = ConnectionState::STATE_DISCONNECTED;
		QMessageBox::warning(this, "Connection Error", QString("Could not connect to server.\nThe error was: %1").arg(message));
		ui.btnConnect->setText("Connect");
		ui.btnConnect->setEnabled(true);
	}
}

void Client::uiFocusOnOverviewTab() {
	ui.tabWidget->setCurrentWidget(ui.tabOverview);
}

void Client::listContactsOnDoubleClick(QListWidgetItem* item) {
	ContactListWidgetItem* clwi = dynamic_cast<ContactListWidgetItem*>(item);

	if (clwi != nullptr) {
		ChatTab* tab = nullptr;
		if (clwi->getContact()->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
			IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());
			tab = MessageCenter::getInstance()->ensureTabOpenForIdentityContact(ic->getContactId());
		} else if (clwi->getContact()->getContactType() == Contact::ContactType::CONTACT_GROUP) {
			GroupContact* gc = dynamic_cast<GroupContact*>(clwi->getContact());
			tab = MessageCenter::getInstance()->ensureTabOpenForGroupContact(gc->getGroupId());
		} else {
			LOGGER()->warn("Could not determine the type of element the user double clicked on in the contacts list.");
			return;
		}
		
		if (tab != nullptr) {
			ui.tabWidget->setCurrentWidget(tab);
		} else {
			LOGGER()->warn("Could not open tab for user or group the user double clicked on in the contacts list.");
		}
	}	
}	

void Client::listContactsOnContextMenu(QPoint const& pos) {
	QPoint globalPos = ui.listContacts->viewport()->mapToGlobal(pos);

	QListWidgetItem* listItem = ui.listContacts->itemAt(pos);
	ContactListWidgetItem* clwi = dynamic_cast<ContactListWidgetItem*>(listItem);
	if (clwi != nullptr) {
		QMenu listContactsContextMenu;

		QAction* actionHeadline = nullptr;
		QAction* actionEdit = nullptr;
		QAction* actionOpenClose = nullptr;
		QAction* actionRequestSync = nullptr;

		bool isChatWindowOpen = false;
		bool isIdentityContact = false;
		bool isGroupSelfOwned = false;
		ChatTab* tab = nullptr;
		if (clwi->getContact()->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
			IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());
			isIdentityContact = true;

			actionHeadline = new QAction(QString("Identity: %1").arg(ic->getContactId().toQString()), &listContactsContextMenu);
			listContactsContextMenu.addAction(actionHeadline);
			actionEdit = new QAction("Edit Contact", &listContactsContextMenu);
			listContactsContextMenu.addAction(actionEdit);

			isChatWindowOpen = MessageCenter::getInstance()->hasTabOpenForIdentityContact(ic->getContactId());
			if (isChatWindowOpen) {
				tab = MessageCenter::getInstance()->ensureTabOpenForIdentityContact(ic->getContactId());
				actionOpenClose = new QAction("Close Chat Window", &listContactsContextMenu);
			} else {
				actionOpenClose = new QAction("Open Chat Window", &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionOpenClose);
		} else {
			GroupContact* gc = dynamic_cast<GroupContact*>(clwi->getContact());
			isIdentityContact = false;

			actionHeadline = new QAction(QString("Group: %1").arg(gc->getGroupId().toQString()), &listContactsContextMenu);
			listContactsContextMenu.addAction(actionHeadline);
			actionEdit = new QAction("Edit Group", &listContactsContextMenu);
			listContactsContextMenu.addAction(actionEdit);

			isChatWindowOpen = MessageCenter::getInstance()->hasTabOpenForGroupContact(gc->getGroupId());
			if (isChatWindowOpen) {
				tab = MessageCenter::getInstance()->ensureTabOpenForGroupContact(gc->getGroupId());
				actionOpenClose = new QAction("Close Chat Window", &listContactsContextMenu);
			} else {
				actionOpenClose = new QAction("Open Chat Window", &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionOpenClose);

			if (gc->getGroupOwner() == ContactRegistry::getInstance()->getSelfContact()->getContactId()) {
				isGroupSelfOwned = true;
				actionRequestSync = new QAction("Force Group Sync", &listContactsContextMenu);
			} else {
				actionRequestSync = new QAction("Request Group Sync", &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionRequestSync);
			if (protocolClient == nullptr || !protocolClient->getIsConnected()) {
				actionRequestSync->setDisabled(true);
			}
		}

		QAction* selectedItem = listContactsContextMenu.exec(globalPos);
		if (selectedItem != nullptr) {
			if (selectedItem == actionEdit) {
				if (isIdentityContact) {
					IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());

					QString const id = ic->getContactId().toQString();
					QString const pubKey = ic->getPublicKey().toString();
					QString const nickname = ic->getNickname();

					ContactEditDialog contactEditDialog(id, pubKey, nickname, this);
					
					int result = contactEditDialog.exec();

					if (result == QDialog::DialogCode::Accepted) {
						QString const newNickname = contactEditDialog.getNickname();
						if (nickname != newNickname) {
							ic->setNickname(newNickname);
						}
					}
				} else {
					showNotYetImplementedInfo();
				}
			} else if (selectedItem == actionOpenClose) {
				if (isChatWindowOpen) {
					if (isIdentityContact) {
						IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());
						MessageCenter::getInstance()->closeTabForIdentityContact(ic->getContactId());
					} else {
						GroupContact* gc = dynamic_cast<GroupContact*>(clwi->getContact());
						MessageCenter::getInstance()->closeTabForGroupContact(gc->getGroupId());
					}
				} else {
					if (isIdentityContact) {
						IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());
						tab = MessageCenter::getInstance()->ensureTabOpenForIdentityContact(ic->getContactId());
					} else {
						GroupContact* gc = dynamic_cast<GroupContact*>(clwi->getContact());
						tab = MessageCenter::getInstance()->ensureTabOpenForGroupContact(gc->getGroupId());
					}

					if (tab != nullptr) {
						ui.tabWidget->setCurrentWidget(tab);
					}
				}
			} else if (!isIdentityContact && (selectedItem == actionRequestSync) && (actionRequestSync != nullptr)) {
				if (protocolClient == nullptr || !protocolClient->getIsConnected()) {
					return;
				}

				GroupContact* gc = dynamic_cast<GroupContact*>(clwi->getContact());
				if (isGroupSelfOwned) {
					QMetaObject::invokeMethod(protocolClient, "resendGroupSetup", Qt::QueuedConnection, Q_ARG(GroupId const&, gc->getGroupId()));
				} else {
					QMetaObject::invokeMethod(protocolClient, "requestGroupSync", Qt::QueuedConnection, Q_ARG(GroupId const&, gc->getGroupId()));
				}
			}
		}
	}
}

void Client::chatTabWidgetOnCurrentTabChanged(int index) {
	// Not the overview-Tab
	if (ui.tabWidget->widget(index) != ui.tabOverview) {
		ChatTab* currentTab = dynamic_cast<ChatTab*>(ui.tabWidget->widget(index));
		if (currentTab != nullptr) {
			currentTab->onReceivedFocus();
		}
	}
}

/*
	MENU ENTRIES & HANDLING
*/

void Client::menuFileOptionsOnClick() {
	OptionsDialog optionsDialog(this);
	int result = optionsDialog.exec();
}

void Client::menuFileExitOnClick() {
	this->close();
}

void Client::menuAboutLicenseOnClick() {
	LicenseDialog* licenseDialog = new LicenseDialog(this);
	licenseDialog->exec();
}

void Client::menuAboutAboutOnClick() {
	QMessageBox::about(this, "OpenMittsu - About", QString("<h2>OpenMittsu</h2><br><br>%1<br>%2<br><br>An open source chat client for Threema-style end-to-end encrypted chat networks.<br><br>This project is in no way connected, affiliated or endorsed with/by Threema GmbH.<br><br>Copyright (C) 2015-16 by Philipp Berger<br>This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.<br>See LICENSE for further information.<br><br>Don't be a jerk!").arg(QString::fromStdString(Version::longVersionString())).arg(QString::fromStdString(Version::buildInfo())));
}

void Client::menuAboutAboutQtOnClick() {
	QMessageBox::aboutQt(this, "About Qt");
}

void Client::menuGroupAddOnClick() {
	if (clientConfiguration == nullptr) {
		QMessageBox::warning(this, "No Identity loaded", "Before you can use this feature you need to load a \"client configuration\" from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else if (protocolClient == nullptr || !protocolClient->getIsConnected()) {
		QMessageBox::warning(this, "Not connected to a server", "Before you can use this feature you need to connect to a server.");
	} else {
		GroupCreationWizard groupCreationWizard(clientConfiguration, protocolClient, this);
		groupCreationWizard.exec();
	}
}

void Client::menuAboutStatisticsOnClick() {
	if ((clientConfiguration == nullptr) || (protocolClient == nullptr) || (!protocolClient->getIsConnected())) {
		QMessageBox::warning(this, "OpenMittsu - Statistics", "Not connected, can not show session statistics.");
	} else {
		QDateTime now = QDateTime::currentDateTime();
		quint64 seconds = protocolClient->getConnectedSince().secsTo(now);
		QMessageBox::information(this, "OpenMittsu - Statistics", QString("Current session:\n\nTime connected: %1\nSend: %2 Bytes\nReceived: %3 Bytes\nMessages send: %4\nMessages received: %5").arg(formatDuration(seconds)).arg(QString::number(protocolClient->getSendBytesCount(), 10)).arg(QString::number(protocolClient->getReceivedBytesCount(), 10)).arg(QString::number(protocolClient->getSendMessagesCount(), 10)).arg(QString::number(protocolClient->getReceivedMessagesCount(), 10)));
	}
}

void Client::menuGroupEditOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuGroupLeaveOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuContactAddOnClick() {
	ContactAddDialog contactAddDialog(this);
	int result = contactAddDialog.exec();

	if (result == QDialog::DialogCode::Accepted) {
		QString const identityString = contactAddDialog.getIdentity();
		QString const nickname = contactAddDialog.getNickname();
		if (identityString.isEmpty() || (identityString.size() != 8)) {
			QMessageBox::warning(this, "Could not add Contact", "The identity entered is not well formed.\nNo Contact has been added.");
		} else if (contactRegistry->hasIdentity(ContactId(identityString.toUtf8()))) {
			QMessageBox::warning(this, "Could not add Contact", "The identity entered is already known.\nNo Contact has been changed.");
		} else {
			try {
				ContactId const contactId(identityString.toUtf8());
				IdentityReceiverCallbackTask ir(serverConfiguration, contactId);
				
				QEventLoop eventLoop;
				OPENMITTSU_CONNECT(&ir, finished(CallbackTask*), &eventLoop, quit());
				ir.start();
				eventLoop.exec();

				if (!ir.hasFinished() || !ir.hasFinishedSuccessfully()) {
					QMessageBox::warning(this, "Could not add Contact", QString("Error while downloading public-key from identity servers: %1\nNo Contact has been added.").arg(ir.getErrorMessage()));
				} else {
					IdentityContact* identityContact = new IdentityContact(contactId, ir.getFetchedPublicKey());
					contactRegistry->addIdentity(identityContact);
					if (!nickname.isEmpty()) {
						contactRegistry->getIdentity(contactId)->setNickname(nickname);
					}

					if (protocolClient != nullptr) {
						QMetaObject::invokeMethod(protocolClient, "addContact", Qt::QueuedConnection, Q_ARG(ContactId, contactId), Q_ARG(PublicKey, ir.getFetchedPublicKey()));
					}

					QMessageBox::information(this, "Contact added", QString("Contact successfully added!\nIdentity: %1\nPublic Key: %2\n").arg(contactId.toQString()).arg(ir.getFetchedPublicKey().toString()));
				}
			} catch (ProtocolErrorException& pee) {
				QMessageBox::warning(this, "Could not add Contact", QString("Error while downloading public-key from identity servers: %1\nNo Contact has been added.").arg(pee.what()));
			}
		}
	}
}

void Client::menuContactEditOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuContactDeleteOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuContactSaveToFileOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuIdentityShowFingerprintOnClick() {
	if (clientConfiguration == nullptr) {
		QMessageBox::warning(this, "No Identity loaded", "Before you can use this feature you need to load a \"client configuration\" from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		FingerprintDialog fingerprintDialog(clientConfiguration, this);
		fingerprintDialog.exec();
	}
}

void Client::menuIdentityShowPublicKeyOnClick() {
	if (clientConfiguration == nullptr) {
		QMessageBox::warning(this, "No Identity loaded", "Before you can use this feature you need to load a \"client configuration\" from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		ShowIdentityAndPublicKeyDialog showIdentityAndPublicKeyDialog(clientConfiguration, this);
		showIdentityAndPublicKeyDialog.exec();
	}
}

void Client::menuIdentityCreateBackupOnClick() {
	if (clientConfiguration == nullptr) {
		QMessageBox::warning(this, "No Identity loaded", "Before you can use this feature you need to load a \"client configuration\" from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		BackupCreationWizard backupCreationWizard(clientConfiguration, this);
		backupCreationWizard.exec();
	}
}

void Client::menuIdentityLoadBackupOnClick() {
	LoadBackupWizard loadBackupWizard(this);
	loadBackupWizard.exec();
}

QString Client::formatDuration(quint64 duration) const {
	QString const result(QStringLiteral("%1 days, %2:%3:%4"));
	quint64 seconds = duration;

	quint64 const days = (seconds / 86400);
	seconds = (seconds % 86400);
	quint64 const hours = (seconds / 3600);
	seconds = (seconds % 3600);
	quint64 const minutes = (seconds / 60);
	seconds = (seconds % 60);

	return result.arg(days).arg(QString::number(hours, 10), 2, '0').arg(QString::number(minutes, 10), 2, '0').arg(QString::number(seconds, 10), 2, '0');
}

void Client::connectionTimerOnTimer() {
	if ((protocolClient != nullptr) && (protocolClient->getIsConnected())) {
		QString const text(QStringLiteral("Connected (since %1)"));
		QDateTime const now = QDateTime::currentDateTime();
		quint64 const seconds = protocolClient->getConnectedSince().secsTo(now);

		ui.lblStatus->setText(text.arg(formatDuration(seconds)));
	} else {
		QString const text(QStringLiteral("Not connected"));
		ui.lblStatus->setText(text);
	}
}

void Client::showNotYetImplementedInfo() {
	QMessageBox::information(this, "Not yet implemented!", "Sorry!\nThis feature is not yet implemented.");
}
