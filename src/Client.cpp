#include <QtWidgets>
#include <QtNetwork>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QSqlDatabase>

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "src/Client.h"

#include "src/backup/BackupReader.h"

#include "src/crypto/FullCryptoBox.h"

#include "src/database/SimpleDatabase.h"

#include "src/dataproviders/BackedContactAndGroupPool.h"
#include "src/dataproviders/KeyRegistry.h"
#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/SimpleGroupCreationProcessor.h"

#include "src/dialogs/ContactAddDialog.h"
#include "src/dialogs/ContactEditDialog.h"
#include "src/dialogs/FingerprintDialog.h"
#include "src/dialogs/LicenseDialog.h"
#include "src/dialogs/OptionsDialog.h"
#include "src/dialogs/ShowIdentityAndPublicKeyDialog.h"
#include "src/dialogs/UpdaterDialog.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InvalidPasswordOrDatabaseException.h"
#include "src/exceptions/ProtocolErrorException.h"

#include "src/network/ServerConfiguration.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/GroupRegistry.h"

#include "src/utility/LegacyContactImporter.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/ThreadDeleter.h"
#include "src/utility/Version.h"

#include "src/tasks/IdentityReceiverCallbackTask.h"
#include "src/tasks/CheckFeatureLevelCallbackTask.h"
#include "src/tasks/CheckContactActivityStatusCallbackTask.h"
#include "src/tasks/SetFeatureLevelCallbackTask.h"

#include "src/widgets/SimpleContactChatTab.h"
#include "src/widgets/SimpleGroupChatTab.h"
#include "src/widgets/SimpleTabController.h"
#include "src/widgets/ContactListWidgetItem.h"
#include "src/widgets/GroupListWidgetItem.h"

#include "src/wizards/BackupCreationWizard.h"
#include "src/wizards/GroupCreationWizard.h"
#include "src/wizards/LoadBackupWizard.h"
#include "src/wizards/FirstUseWizard.h"

#include "Config.h"

#include "ui_main.h"

Client::Client(QWidget* parent) : QMainWindow(parent),
m_ui(std::make_unique<Ui::MainWindow>()),
m_protocolClient(nullptr),
m_protocolClientThread(this),
m_connectionTimer(this),
m_unreadMessagesIconBlinkTimer(this),
m_unreadMessagesIconBlinkState(false),
m_updater(),
m_connectionState(ConnectionState::STATE_DISCONNECTED),
m_tabController(nullptr),
m_messageCenterThread(),
m_messageCenterPointerAuthority(),
m_messageCenterWrapper(&m_messageCenterPointerAuthority),
m_serverConfiguration(std::make_shared<openmittsu::network::ServerConfiguration>()),
m_optionMaster(nullptr),
m_databaseThread(),
m_databasePointerAuthority(),
m_databaseWrapper(&m_databasePointerAuthority),
m_audioNotifier(std::make_shared<openmittsu::utility::AudioNotification>()),
m_optionTryEmptyPassword(false),
m_optionAutoConnect(false),
m_optionMinimize(false),
m_optionUsePasswordFile(false),
m_optionPasswordFromFile(),
m_optionUseDatabaseFile(),
m_optionDatabaseFile() {
	m_ui->setupUi(this);

	// Parse commandline options
	bool showHelp = false;
	QHash<QString, bool*> knownOptions = { 
		{"--openmittsu-nopassword", &m_optionTryEmptyPassword }, 
		{"--openmittsu-password-file", &m_optionUsePasswordFile }, 
		{"--openmittsu-database-file", &m_optionUseDatabaseFile },
		{"--openmittsu-autoconnect", &m_optionAutoConnect}, 
		{"--openmittsu-minimize", &m_optionMinimize}, 
		{"--help", &showHelp} 
	};
	QStringList arguments = QCoreApplication::arguments();

	for (int i = 0; i < arguments.size(); ++i) {
		QString const& arg = arguments.at(i);
		auto const it = knownOptions.constFind(arg.toLower());
		if (it != knownOptions.constEnd()) {
			// Found option
			bool* option = it.value();
			*option = true;
			if (it.key().compare("--openmittsu-password-file") == 0) {
				if ((i + 1) >= arguments.size()) {
					std::cerr << "Missing required argument for --openmittsu-password-file!" << std::endl;
					std::exit(-10);
				}
				
				QString passwordFileName = arguments.at(i + 1);
				QFile passwordFile(passwordFileName);
				if (!passwordFile.open(QFile::ReadOnly)) {
					QMessageBox::information(this, tr("Password file"), tr("Tried getting the database password from file '%1', but it does not exist or is not readable.").arg(passwordFileName));
					LOGGER()->error("Tried getting the database password from file '{}', but it does not exist or is not readable.", passwordFileName.toStdString());
					std::exit(-11);
				}
				m_optionPasswordFromFile = QString(passwordFile.readAll());
				LOGGER()->info("Loaded password for OpenMittsu database from file.");

				++i; // Skip parsing the string argument
			} else if (it.key().compare("--openmittsu-database-file") == 0) {
				if ((i + 1) >= arguments.size()) {
					std::cerr << "Missing required argument for --openmittsu-database-file!" << std::endl;
					std::exit(-12);
				}

				m_optionDatabaseFile = arguments.at(i + 1);
				if (!QFile::exists(m_optionDatabaseFile)) {
					QMessageBox::information(this, tr("Database file"), tr("Tried finding the database file '%1', but it does not exist or is not readable.").arg(m_optionDatabaseFile));
					LOGGER()->error("Tried finding the database file '{}', but it does not exist or is not readable.", m_optionDatabaseFile.toStdString());
					std::exit(-13);
				}

				++i; // Skip parsing the string argument
			}
		}
	}
	
	if (showHelp) {
		std::cout << "OpenMittsu" << std::endl;
		std::cout << "----------" << std::endl;
		std::cout << "A cross-platform open source implementation and desktop client for the Threema Messenger App." << std::endl;
		std::cout << std::endl;
		std::cout << openmittsu::utility::Version::longVersionString() << std::endl;
		std::cout << std::endl;
		std::cout << "Available command-line options:" << std::endl;
		std::cout << std::endl;
		std::cout << " --openmittsu-autoconnect - Auto-connect after starting." << std::endl;
		std::cout << " --openmittsu-database-file PATH/FILENAME - Load the database FILENAME. Usually a path ending in 'openmittsu.sqlite'." << std::endl;
		std::cout << " --openmittsu-minimize - Minimize the window after starting." << std::endl;
		std::cout << " --openmittsu-nopassword - Try opening the database with the empty password." << std::endl;
		std::cout << " --openmittsu-password-file PATH/FILENAME - Read the file and use its contents as the password to the database. Look out for extra new-lines!" << std::endl;
		std::cout << std::endl;
		std::cout << "Good bye!" << std::endl;
		std::exit(0);
	}

	m_tabController = std::make_shared<openmittsu::widgets::SimpleTabController>(m_ui->tabWidget);

	bool messageCenterCreationSuccess = false;
	if ((!QMetaObject::invokeMethod(m_messageCenterThread.getQObjectPtr(), "createMessageCenter", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, messageCenterCreationSuccess), Q_ARG(openmittsu::database::DatabaseWrapperFactory const&, m_databasePointerAuthority.getDatabaseWrapperFactory()))) || (!messageCenterCreationSuccess)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not create MessageCenter, terminating.";
	}
	m_messageCenterPointerAuthority.setMessageCenter(m_messageCenterThread.getWorker().getMessageCenter());

	m_connectionTimer.start(500);
	OPENMITTSU_CONNECT(&m_connectionTimer, timeout(), this, connectionTimerOnTimer());
	OPENMITTSU_CONNECT(&m_unreadMessagesIconBlinkTimer, timeout(), this, unreadMessagesIconBlinkTimerOnTimer());
	OPENMITTSU_CONNECT(&m_messageCenterWrapper, newUnreadMessageAvailableContact(openmittsu::protocol::ContactId const&), this, onMessageCenterHasUnreadMessageContact(openmittsu::protocol::ContactId const&));
	OPENMITTSU_CONNECT(&m_messageCenterWrapper, newUnreadMessageAvailableGroup(openmittsu::protocol::GroupId const&), this, onMessageCenterHasUnreadMessageGroup(openmittsu::protocol::GroupId const&));

	OPENMITTSU_CONNECT(&m_databaseWrapper, gotDatabase(), this, onDatabaseUpdated());
	OPENMITTSU_CONNECT(&m_databaseWrapper, contactChanged(openmittsu::protocol::ContactId const&), this, onDatabaseContactChanged(openmittsu::protocol::ContactId const&));
	OPENMITTSU_CONNECT(&m_databaseWrapper, groupChanged(openmittsu::protocol::GroupId const&), this, onDatabaseGroupChanged(openmittsu::protocol::GroupId const&));
	OPENMITTSU_CONNECT(&m_databaseWrapper, receivedNewContactMessage(openmittsu::protocol::ContactId const&), this, onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const&));
	OPENMITTSU_CONNECT(&m_databaseWrapper, receivedNewGroupMessage(openmittsu::protocol::GroupId const&), this, onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const&));

	// Check whether QSqlCipher is available
	if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLCIPHER"))) {
#ifdef OPENMITTSU_CONFIG_ALLOW_MISSING_QSQLCIPHER
		QMessageBox::warning(this, tr("Database driver not available"), tr("openMittsu relies on SqlCipher and QSqlCipher for securely storing the database.\nThe QSQLCIPHER driver is not available. It should reside in the sqldrivers\\ subdirectory of openMittsu.\nWe will use the unencrypted SQLITE driver instead."));
#else
		QMessageBox::critical(this, tr("Database driver not available"), tr("openMittsu relies on SqlCipher and QSqlCipher for securely storing the database.\nThe QSQLCIPHER driver is not available. It should reside in the sqldrivers\\ subdirectory of openMittsu.\nSince no encryption is available, OpenMittsu will now terminate."));
#endif
	}

	// Check whether OpenSSL is available
	{
		QSslCertificate const testCertificate = QSslCertificate(QByteArray::fromBase64(m_serverConfiguration->getApiServerCertificateAsBase64().toLocal8Bit()), QSsl::EncodingFormat::Pem);
		if (testCertificate.isNull()) {
			QMessageBox::critical(this, tr("OpenSSL support unavailable"), tr("openMittsu relies on OpenSSL for accessing https resources and it can not function without it.\nMake sure to install runtime libraries from your distribution.\nOn Windows, ssleay32.dll and libssl32.dll in the appropriate architecture are required in the same folder as openMittsu.exe"));
			QApplication::exit(-1);
			throw openmittsu::exceptions::InternalErrorException() << "No OpenSSL support available, terminating.";
		} else {
			LOGGER_DEBUG("OpenSSL context seems to work, could load certificate.");
		}
	}

	// Load stored settings
	this->m_optionMaster = std::make_shared<openmittsu::options::OptionMaster>(m_databaseWrapper);

	OPENMITTSU_CONNECT(m_ui->btnConnect, clicked(), this, btnConnectOnClick());
	OPENMITTSU_CONNECT(m_ui->btnOpenDatabase, clicked(), this, btnOpenDatabaseOnClick());

	m_ui->listContacts->setContextMenuPolicy(Qt::CustomContextMenu);
	m_ui->listGroups->setContextMenuPolicy(Qt::CustomContextMenu);
	OPENMITTSU_CONNECT(m_ui->listContacts, itemDoubleClicked(QListWidgetItem*), this, listContactsOnDoubleClick(QListWidgetItem*));
	OPENMITTSU_CONNECT(m_ui->listContacts, customContextMenuRequested(const QPoint&), this, listContactsOnContextMenu(const QPoint&));
	OPENMITTSU_CONNECT(m_ui->listGroups, itemDoubleClicked(QListWidgetItem*), this, listGroupsOnDoubleClick(QListWidgetItem*));
	OPENMITTSU_CONNECT(m_ui->listGroups, customContextMenuRequested(const QPoint&), this, listGroupsOnContextMenu(const QPoint&));

	// Menus
	OPENMITTSU_CONNECT(m_ui->actionLicense, triggered(), this, menuAboutLicenseOnClick());
	OPENMITTSU_CONNECT(m_ui->actionAbout, triggered(), this, menuAboutAboutOnClick());
	OPENMITTSU_CONNECT(m_ui->actionAbout_Qt, triggered(), this, menuAboutAboutQtOnClick());
	OPENMITTSU_CONNECT(m_ui->actionAdd_a_Contact, triggered(), this, menuContactAddOnClick());
	OPENMITTSU_CONNECT(m_ui->actionDelete_a_Contact, triggered(), this, menuContactDeleteOnClick());
	OPENMITTSU_CONNECT(m_ui->actionEdit_a_Contact, triggered(), this, menuContactEditOnClick());
	OPENMITTSU_CONNECT(m_ui->actionSave_to_file, triggered(), this, menuContactSaveToFileOnClick());
	//OPENMITTSU_CONNECT(m_ui->actionShow_Emoji_Input, triggered(bool), this, menuViewShowEmojiTabOnTriggered(bool));
	OPENMITTSU_CONNECT(m_ui->actionAdd_Group, triggered(), this, menuGroupAddOnClick());
	OPENMITTSU_CONNECT(m_ui->actionEdit_Group, triggered(), this, menuGroupEditOnClick());
	OPENMITTSU_CONNECT(m_ui->actionLeave_Group, triggered(), this, menuGroupLeaveOnClick());
	OPENMITTSU_CONNECT(m_ui->actionCreate_Backup, triggered(), this, menuIdentityCreateBackupOnClick());
	OPENMITTSU_CONNECT(m_ui->actionLoad_Backup, triggered(), this, menuIdentityLoadBackupOnClick());
	OPENMITTSU_CONNECT(m_ui->actionShow_Fingerprint, triggered(), this, menuIdentityShowFingerprintOnClick());
	OPENMITTSU_CONNECT(m_ui->actionShow_Public_Key, triggered(), this, menuIdentityShowPublicKeyOnClick());
	OPENMITTSU_CONNECT(m_ui->actionImport_legacy_contacts_and_groups, triggered(), this, menuDatabaseImportLegacyContactsAndGroupsOnClick());
	OPENMITTSU_CONNECT(m_ui->actionStatistics, triggered(), this, menuAboutStatisticsOnClick());
	OPENMITTSU_CONNECT(m_ui->actionOptions, triggered(), this, menuFileOptionsOnClick());
	OPENMITTSU_CONNECT(m_ui->actionShow_First_Use_Wizard, triggered(), this, menuFileShowFirstUseWizardOnClick());
	OPENMITTSU_CONNECT(m_ui->actionExit, triggered(), this, menuFileExitOnClick());

	m_protocolClientThread.start();
	OPENMITTSU_CONNECT(&m_protocolClientThread, finished(), this, threadFinished());

#ifndef OPENMITTSU_CONFIG_DISABLE_VERSION_UPDATE_CHECK
	// Call Updater
	OPENMITTSU_CONNECT_QUEUED(&m_updater, foundNewVersion(int, int, int, int, QString, QString, QString), this, updaterFoundNewVersion(int, int, int, int, QString, QString, QString));
	QTimer::singleShot(0, &m_updater, SLOT(start()));
#endif

	QTimer::singleShot(0, this, SLOT(delayedStartup()));
}

Client::~Client() {
	if (m_protocolClient != nullptr) {
		m_protocolClient = nullptr;
	}

	if (m_protocolClientThread.isRunning()) {
		m_protocolClientThread.quit();
	}
	while (!m_protocolClientThread.isFinished()) {
		QThread::currentThread()->wait(10);
	}
}

void Client::delayedStartup() {
	QString const databaseFile = m_optionMaster->getOptionAsQString(openmittsu::options::Options::FILEPATH_DATABASE);
	QString const legacyClientConfiguration = m_optionMaster->getOptionAsQString(openmittsu::options::Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION);
	bool showFirstUseWizard = false;
	bool showFromBackupWizard = false;

	if (m_optionUseDatabaseFile && QFile::exists(m_optionDatabaseFile)) {
		LOGGER()->info("Loading database from command-line option provided location '{}'.", m_optionDatabaseFile.toStdString());
		openDatabaseFile(m_optionDatabaseFile);
	} else if (!databaseFile.isEmpty()) {
		if (!QFile::exists(databaseFile)) {
			LOGGER_DEBUG("Removing key \"FILEPATH_DATABASE\" from stored settings as the file is not valid.");
			m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_DATABASE, "");
		} else {
			openDatabaseFile(databaseFile);
		}
	} else {
		if (!legacyClientConfiguration.isEmpty() && QFile::exists(legacyClientConfiguration)) {
			showFromBackupWizard = true;
		} else {
			showFirstUseWizard = true;
		}
	}

	contactRegistryOnIdentitiesChanged();

	if (showFirstUseWizard) {
		menuFileShowFirstUseWizardOnClick();
	} else if (showFromBackupWizard) {
		auto const resultButton = QMessageBox::question(this, tr("Legacy client configuration found"), tr("Welcome.\nIt seems that you used an older version of openMittsu before that used a plaintext client configuration file to store your ID.\nThis has been replaced by an encrypted database storing both your ID, contacts, groups and messages.\nIf you want, your legacy ID file can be converted into a modern openMittsu database. Click yes to import the old file."));
		if (resultButton == QMessageBox::StandardButton::Yes) {
			menuIdentityLoadBackupOnClick(legacyClientConfiguration);
			m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION, "");
		}
	}

	if (m_databaseWrapper.hasDatabase()) {
		QString const legacyContactsFile = m_optionMaster->getOptionAsQString(openmittsu::options::Options::FILEPATH_LEGACY_CONTACTS_DATABASE);
		m_optionMaster->forceInitialization();
		if (!legacyContactsFile.isEmpty() && QFile::exists(legacyContactsFile) && !m_optionMaster->getOptionAsBool(openmittsu::options::Options::BOOLEAN_IGNORE_LEGACY_CONTACTS_DATABASE)) {
			auto const resultButton = QMessageBox::question(this, tr("Legacy contacts file found"), tr("Welcome.\nIt seems that you used an older version of openMittsu before that used a plaintext contacts file to store your contacts and groups.\nThis has been replaced by an encrypted database storing both your ID, contacts, groups and messages.\nIf you want, your legacy contacts file can be imported into your openMittsu database.\nClick \"Yes\" to import the old file (located at %1), \"No\" to leave it for now, \"Ignore\" to permanently ignore the file (can be changed in the options) or \"Discard\" to delete the reference to the old file.\nYou can always come back later and import it via Database -> Import openMittsu....").arg(legacyContactsFile), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Ignore | QMessageBox::StandardButton::Discard);
			if (resultButton == QMessageBox::StandardButton::Yes) {
				menuDatabaseImportLegacyContactsAndGroupsOnClick(legacyContactsFile);
				m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_LEGACY_CONTACTS_DATABASE, "");
			} else if (resultButton == QMessageBox::StandardButton::Ignore) {
				m_optionMaster->setOption(openmittsu::options::Options::BOOLEAN_IGNORE_LEGACY_CONTACTS_DATABASE, true);
			} else if (resultButton == QMessageBox::StandardButton::Discard) {
				m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_LEGACY_CONTACTS_DATABASE, "");
			}
		}
	}

	// Restore Window location and size
	restoreGeometry(m_optionMaster->getOptionAsQByteArray(openmittsu::options::Options::BINARY_MAINWINDOW_GEOMETRY));
	restoreState(m_optionMaster->getOptionAsQByteArray(openmittsu::options::Options::BINARY_MAINWINDOW_STATE));

	// Autoconnect if asked to
	if (m_optionAutoConnect) {
		QTimer::singleShot(0, this, SLOT(btnConnectOnClick()));
	}

	// Minimize if asked to
	if (m_optionMinimize) {
		this->showMinimized();
	}
}

void Client::closeEvent(QCloseEvent* event) {
	// Save location and size of window
	m_optionMaster->setOption(openmittsu::options::Options::BINARY_MAINWINDOW_GEOMETRY, saveGeometry());
	m_optionMaster->setOption(openmittsu::options::Options::BINARY_MAINWINDOW_STATE, saveState());

	// Close server connection and tear down threads
	if (m_protocolClient != nullptr) {
		// Deletes the client
		m_protocolClient = nullptr;
	}

	if (m_protocolClientThread.isRunning()) {
		m_protocolClientThread.quit();
	}

	QMainWindow::closeEvent(event);
}

void Client::setupProtocolClient() {
	if (m_protocolClient != nullptr) {
		OPENMITTSU_DISCONNECT(m_protocolClient.get(), connectToFinished(int, QString), this, protocolClientOnConnectToFinished(int, QString));
		OPENMITTSU_DISCONNECT(m_protocolClient.get(), readyConnect(), this, protocolClientOnReadyConnect());
		OPENMITTSU_DISCONNECT(m_protocolClient.get(), lostConnection(), this, protocolClientOnLostConnection());
		OPENMITTSU_DISCONNECT(m_protocolClient.get(), duplicateIdUsageDetected(), this, protocolClientOnDuplicateIdUsageDetected());


		m_messageCenterWrapper.setNetworkSentMessageAcceptor(nullptr);

		if (m_protocolClient->getIsConnected()) {
			m_protocolClient->disconnectFromServer();
		}

		QEventLoop eventLoop;
		OPENMITTSU_CONNECT(m_protocolClient.get(), teardownComplete(), &eventLoop, quit());
		if (!QMetaObject::invokeMethod(m_protocolClient.get(), "teardown", Qt::BlockingQueuedConnection)) {
			throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method teardown in " << __FILE__ << "  at line " << __LINE__ << ".";
		}

		m_protocolClient.reset();
	}

	if (!m_databaseWrapper.hasDatabase()) {
		LOGGER()->critical("Can not set up ProtocolClient as no database is available, terminating setup early.");
		return;
	}

	openmittsu::protocol::ContactId const selfContactId = m_databaseWrapper.getSelfContact();
	openmittsu::database::ContactData const selfContactData = m_databaseWrapper.getContactData(selfContactId, false);

	QString const nickname = selfContactData.nickName;
	if (nickname.compare(QStringLiteral("You"), Qt::CaseInsensitive) == 0) {
		LOGGER()->info("Using only ID as PushFromID token (for iOS Push Receivers).");
		m_protocolClient = std::make_unique<openmittsu::network::ProtocolClient>(m_databasePointerAuthority.getDatabaseWrapperFactory(), selfContactId, m_serverConfiguration, openmittsu::options::OptionReaderFactory(m_databasePointerAuthority.getDatabaseWrapperFactory()), m_messageCenterPointerAuthority.getMessageCenterWrapperFactory(), openmittsu::protocol::PushFromId(selfContactId));
	} else {
		m_protocolClient = std::make_unique<openmittsu::network::ProtocolClient>(m_databasePointerAuthority.getDatabaseWrapperFactory(), selfContactId, m_serverConfiguration, openmittsu::options::OptionReaderFactory(m_databasePointerAuthority.getDatabaseWrapperFactory()), m_messageCenterPointerAuthority.getMessageCenterWrapperFactory(), openmittsu::protocol::PushFromId(nickname));
		LOGGER()->info("Using nickname \"{}\" as PushFromID token (for iOS Push Receivers).", nickname.toStdString());
	}

	m_protocolClient->moveToThread(&m_protocolClientThread);

	OPENMITTSU_CONNECT(m_protocolClient.get(), connectToFinished(int, QString), this, protocolClientOnConnectToFinished(int, QString));
	OPENMITTSU_CONNECT(m_protocolClient.get(), readyConnect(), this, protocolClientOnReadyConnect());
	OPENMITTSU_CONNECT(m_protocolClient.get(), lostConnection(), this, protocolClientOnLostConnection());
	OPENMITTSU_CONNECT(m_protocolClient.get(), duplicateIdUsageDetected(), this, protocolClientOnDuplicateIdUsageDetected());

	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(m_protocolClient.get(), setupDone(), &eventLoop, quit());

	if (!QMetaObject::invokeMethod(m_protocolClient.get(), "setup", Qt::QueuedConnection)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method setup in " << __FILE__ << "  at line " << __LINE__ << ".";
	}
	eventLoop.exec(); // blocks until "finished()" has been called

	m_messageCenterWrapper.setNetworkSentMessageAcceptor(std::make_shared<openmittsu::dataproviders::NetworkSentMessageAcceptor>(m_protocolClient));
}

void Client::threadFinished() {
	LOGGER_DEBUG("Client::threadFinished - the worker thread finished.");
	if (m_protocolClient != nullptr) {
		if (!QMetaObject::invokeMethod(m_protocolClient.get(), "teardown", Qt::QueuedConnection)) {
			throw openmittsu::exceptions::InternalErrorException() << "Could not invoke method teardown in " << __FILE__ << "  at line " << __LINE__ << ".";
		}
		m_protocolClient->deleteLater();
		m_protocolClient.reset();
	}
	LOGGER()->critical("Since the worker thread just terminated, everything will close now.");
	this->close();
}

void Client::updaterFoundNewVersion(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString gitHash, QString channel, QString link) {
	openmittsu::dialogs::UpdaterDialog updateDialog(versionMajor, versionMinor, versionPatch, commitsSinceTag, gitHash, channel, link, this);
	updateDialog.exec();
}

void Client::updateDatabaseInfo(QString const& currentFileName) {
	m_ui->lblDatabase->setText(currentFileName);
	if (m_databaseThread.getWorker().hasDatabase()) {
		if (!m_databaseWrapper.hasDatabase()) {
			LOGGER_DEBUG("Database was set, but wrapper does not announce so.");
			return;
		}
		m_databaseWrapper.enableTimers();
	}
}

void Client::btnConnectOnClick() {
	if (m_connectionState == ConnectionState::STATE_DISCONNECTED) {
		if ((m_serverConfiguration == nullptr) || (!m_databaseWrapper.hasDatabase())) {
			QMessageBox::warning(this, "Can not connect", "Please choose a valid database file first.");
			return;
		}

		m_ui->btnConnect->setEnabled(false);
		m_ui->btnConnect->setText(tr("Connecting..."));

		setupProtocolClient();
		QTimer::singleShot(0, m_protocolClient.get(), SLOT(connectToServer()));
	} else if (m_connectionState == ConnectionState::STATE_CONNECTING) {
		// No click should be possible in this state
		m_ui->btnConnect->setEnabled(false);
	} else if (m_connectionState == ConnectionState::STATE_CONNECTED) {
		m_ui->btnConnect->setEnabled(false);
		m_ui->btnConnect->setText(tr("Disconnecting..."));
		QTimer::singleShot(0, m_protocolClient.get(), SLOT(disconnectFromServer()));
	}
}

bool Client::validateDatabaseFile(QString const& databaseFileName, QString const& password, bool quiet) {
	try {
		QFileInfo fileInfo(databaseFileName);
		if (!fileInfo.exists()) {
			return false;
		}

		QDir const folder(fileInfo.absolutePath());
		if (!folder.exists()) {
			return false;
		}

		openmittsu::database::SimpleDatabase db(databaseFileName, password, folder, false);
		return true;
	} catch (openmittsu::exceptions::BaseException& iex) {
		if (!quiet) {
			QMessageBox::warning(this, tr("Invalid Database file"), QString(tr("The selected file can not be used.\nReason: %1")).arg(iex.what()));
		}
	}
	return false;
}

void Client::btnOpenDatabaseOnClick() {
	QString const fileName = QFileDialog::getOpenFileName(this, tr("Select an openMittsu database file"), "", "*.sqlite");
	if (!fileName.isEmpty() && QFile::exists(fileName)) {
		openDatabaseFile(fileName);
	}
}

void Client::openDatabaseFile(QString const& fileName) {
	if (!QFile::exists(fileName)) {
		QMessageBox::warning(this, tr("Database file does not exist"), tr("The selected database file \"%1\" does not exist. Please check the access permissions!").arg(fileName));
		return;
	}

	QString password;
	while (true) {
		QString password;
		bool ok = false;
		if (m_optionTryEmptyPassword) {
			password = "";
			ok = true;
		} else if (m_optionUsePasswordFile) {
			password = m_optionPasswordFromFile;
			ok = true;
		} else {
			password = QInputDialog::getText(this, tr("Database password"), tr("Please enter the database password for file \"%1\":").arg(fileName), QLineEdit::Password, QString(), &ok);
			if (password.isNull()) {
				password = QString("");
			}
		}
		if (ok) {
			QDir location(fileName);
			location.cdUp();

			int databaseOpenSuccess = -1;

			if (!QMetaObject::invokeMethod(m_databaseThread.getQObjectPtr(), "openDatabase", Qt::BlockingQueuedConnection, Q_RETURN_ARG(int, databaseOpenSuccess), Q_ARG(QString const&, fileName), Q_ARG(QString const&, password), Q_ARG(QDir const&, location), Q_ARG(bool, false))) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not invoke Database open, terminating.";
			} else if (databaseOpenSuccess != 0) {
				if (databaseOpenSuccess == 1) {
					// Try compat options
					if (!QMetaObject::invokeMethod(m_databaseThread.getQObjectPtr(), "openDatabase", Qt::BlockingQueuedConnection, Q_RETURN_ARG(int, databaseOpenSuccess), Q_ARG(QString const&, fileName), Q_ARG(QString const&, password), Q_ARG(QDir const&, location), Q_ARG(bool, true))) {
						throw openmittsu::exceptions::InternalErrorException() << "Could not invoke Database open, terminating.";
					} else if (databaseOpenSuccess != 0) {
						if (databaseOpenSuccess == 1) {
							if (m_optionTryEmptyPassword) {
								QMessageBox::information(this, tr("Invalid password"), tr("Tried the empty password for opening the database (commandline option set), but it was rejected."));
								m_optionTryEmptyPassword = false;
							} else if (m_optionUsePasswordFile) {
								QMessageBox::information(this, tr("Invalid password"), tr("Tried the password from the given file for opening the database (commandline option set), but it was rejected."));
								m_optionUsePasswordFile = false;
							} else {
								QMessageBox::information(this, tr("Invalid password"), tr("The entered database password was invalid."));
							}
						} else {
							LOGGER_DEBUG("Removing key \"FILEPATH_DATABASE\" from stored settings as the file is not valid.");
							m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_DATABASE, "");
							break;
						}
					}
				} else {
					LOGGER_DEBUG("Removing key \"FILEPATH_DATABASE\" from stored settings as the file is not valid.");
					m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_DATABASE, "");
					break;
				}
			}

			if (databaseOpenSuccess == 0) {
				this->m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_DATABASE, fileName);
				m_databasePointerAuthority.setDatabase(m_databaseThread.getWorker().getDatabase());

				while (!m_databaseWrapper.hasDatabase()) {
					QCoreApplication::processEvents();
					QThread::msleep(25);
				}

				updateDatabaseInfo(fileName);

				contactRegistryOnIdentitiesChanged();
				break;
			}
		} else {
			m_optionMaster->setOption(openmittsu::options::Options::FILEPATH_DATABASE, "");
			break;
		}
	}
}

void Client::contactRegistryOnIdentitiesChanged() {
	LOGGER_DEBUG("Updating contacts list on IdentitiesChanged() signal.");
	m_ui->listContacts->clear();
	m_ui->listGroups->clear();

	if (m_databaseWrapper.hasDatabase()) {
		QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> knownIdentities = m_databaseWrapper.getContactDataAll(false);
		auto it = knownIdentities.constBegin();
		auto const end = knownIdentities.constEnd();

		openmittsu::protocol::ContactId const selfIdentity = m_databaseWrapper.getSelfContact();
		for (; it != end; ++it) {
			openmittsu::protocol::ContactId const contactId = it.key();
			if (contactId == selfIdentity) {
				continue;
			}
			openmittsu::widgets::ContactListWidgetItem* clwi = nullptr;
			if (it->nickName.isNull() || it->nickName.isEmpty()) {
				clwi = new openmittsu::widgets::ContactListWidgetItem(contactId, false, contactId.toQString());
			} else {
				clwi = new openmittsu::widgets::ContactListWidgetItem(contactId, false, it->nickName);
			}
			openmittsu::protocol::AccountStatus const status = it->accountStatus;
			if (status == openmittsu::protocol::AccountStatus::STATUS_INACTIVE) {
				clwi->setBackground(QBrush(QColor::fromRgb(255, 255, 51)));
			} else if (status == openmittsu::protocol::AccountStatus::STATUS_INVALID) {
				clwi->setBackground(QBrush(QColor::fromRgb(250, 128, 114)));
			}

			bool inserted = false;
			for (int i = 0; i < m_ui->listContacts->count(); ++i) {
				if (*clwi < *m_ui->listContacts->item(i)) {
					m_ui->listContacts->insertItem(i, clwi);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				m_ui->listContacts->addItem(clwi);
			}
		}

		// Groups
		QHash<openmittsu::protocol::GroupId, openmittsu::database::GroupData> knownGroups = m_databaseWrapper.getGroupDataAll(false);
		auto itGroups = knownGroups.constBegin();
		auto const endGroups = knownGroups.constEnd();

		for (; itGroups != endGroups; ++itGroups) {
			openmittsu::protocol::GroupId const groupId = itGroups.key();
			openmittsu::widgets::GroupListWidgetItem* glwi = new openmittsu::widgets::GroupListWidgetItem(groupId, false, itGroups->title);

			bool inserted = false;
			for (int i = 0; i < m_ui->listGroups->count(); ++i) {
				if (*glwi < *m_ui->listGroups->item(i)) {
					m_ui->listGroups->insertItem(i, glwi);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				m_ui->listGroups->addItem(glwi);
			}
		}
	}
}

void Client::onHasUnreadMessage(openmittsu::widgets::ChatTab* tab) {
	LOGGER_DEBUG("Activating window for unread messages...");
	if (m_optionMaster->getOptionAsBool(openmittsu::options::Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED)) {
		this->activateWindow();
	}

	if (m_optionMaster->getOptionAsBool(openmittsu::options::Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED)) {
		if (m_audioNotifier) {
			m_audioNotifier->playNotification();
		}
	}

	if (m_optionMaster->getOptionAsBool(openmittsu::options::Options::BOOLEAN_BLINK_ICON_ON_MESSAGE_RECEIVED)) {
		if ((QApplication::activeWindow() == nullptr) && (!m_unreadMessagesIconBlinkTimer.isActive())) {
			m_unreadMessagesIconBlinkState = true;
			setAppIcon(true);
			m_unreadMessagesIconBlinkTimer.setInterval(500);
			m_unreadMessagesIconBlinkTimer.start();
		}
	}
}

void Client::onMessageCenterHasUnreadMessageContact(openmittsu::protocol::ContactId const& contact) {
	// TODO Implement and check with regard to below DB functions
}

void Client::onMessageCenterHasUnreadMessageGroup(openmittsu::protocol::GroupId const& group) {
	// TODO Implement and check with regard to below DB functions
}

void Client::onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const& identity) {
	if (m_tabController && m_databaseWrapper.hasDatabase()) {
		openmittsu::widgets::ChatTab* chatTab = nullptr;
		if (m_tabController->hasTab(identity)) {
			chatTab = m_tabController->getTab(identity);
		} else {
			openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
			m_tabController->openTab(identity, pool.getBackedContact(identity, m_databaseWrapper, m_messageCenterWrapper));
			chatTab = m_tabController->getTab(identity);
		}
		onHasUnreadMessage(chatTab);
	}
}

void Client::onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const& group) {
	if (m_tabController && m_databaseWrapper.hasDatabase()) {
		openmittsu::widgets::ChatTab* chatTab = nullptr;
		if (m_tabController->hasTab(group)) {
			chatTab = m_tabController->getTab(group);
		} else {
			openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
			m_tabController->openTab(group, pool.getBackedGroup(group, m_databaseWrapper, m_messageCenterWrapper));
			chatTab = m_tabController->getTab(group);
		}
		onHasUnreadMessage(chatTab);
	}
}

void Client::onDatabaseUpdated() {
	contactRegistryOnIdentitiesChanged();
}

void Client::onDatabaseContactChanged(openmittsu::protocol::ContactId const& contact) {
	// TODO: Better
	contactRegistryOnIdentitiesChanged();
}

void Client::onDatabaseGroupChanged(openmittsu::protocol::GroupId const& group) {
	// TODO: Better
	contactRegistryOnIdentitiesChanged();
}

void Client::protocolClientOnReadyConnect() {
	LOGGER_DEBUG("In Client: protocolClientOnReadyConnect()");
	m_connectionState = ConnectionState::STATE_DISCONNECTED;
	m_ui->btnConnect->setText("Connect");
	m_ui->btnConnect->setEnabled(true);
	uiFocusOnOverviewTab();
}

void Client::protocolClientOnLostConnection() {
	LOGGER_DEBUG("In Client: protocolClientOnLostConnection()");
	m_connectionState = ConnectionState::STATE_DISCONNECTED;
	m_ui->btnConnect->setText("Connect");
	m_ui->btnConnect->setEnabled(true);
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
		m_connectionState = ConnectionState::STATE_CONNECTED;
		m_ui->btnConnect->setText(tr("Disconnect"));
		m_ui->btnConnect->setEnabled(true);

		// Start checking feature levels and statuses of contacts
		if (m_databaseThread.getWorker().hasDatabase()) {
			QSet<openmittsu::protocol::ContactId> const contactsRequiringAccountStatusCheck = m_databaseWrapper.getContactsRequiringAccountStatusCheck(86400);
			QSet<openmittsu::protocol::ContactId> const contactsRequiringFeatureLevelCheck = m_databaseWrapper.getContactsRequiringFeatureLevelCheck(86400);

			if (contactsRequiringFeatureLevelCheck.size() > 0) {
				openmittsu::tasks::CheckFeatureLevelCallbackTask* taskCheckFeatureLevels = new openmittsu::tasks::CheckFeatureLevelCallbackTask(m_serverConfiguration, contactsRequiringFeatureLevelCheck);
				OPENMITTSU_CONNECT_QUEUED(taskCheckFeatureLevels, finished(openmittsu::tasks::CallbackTask*), this, callbackTaskFinished(openmittsu::tasks::CallbackTask*));
				taskCheckFeatureLevels->start();
			}

			if (contactsRequiringAccountStatusCheck.size() > 0) {
				openmittsu::tasks::CheckContactActivityStatusCallbackTask* taskCheckContactIdStatus = new openmittsu::tasks::CheckContactActivityStatusCallbackTask(m_serverConfiguration, contactsRequiringAccountStatusCheck);
				OPENMITTSU_CONNECT_QUEUED(taskCheckContactIdStatus, finished(openmittsu::tasks::CallbackTask*), this, callbackTaskFinished(openmittsu::tasks::CallbackTask*));
				taskCheckContactIdStatus->start();
			}
		}
	} else {
		m_connectionState = ConnectionState::STATE_DISCONNECTED;
		LOGGER()->warn("Could not connect to server. The error was: {}", message.toStdString());
		QMessageBox::warning(this, "Connection Error", QString("Could not connect to server.\nThe error was: %1").arg(message));
		m_ui->btnConnect->setText(tr("Connect"));
		m_ui->btnConnect->setEnabled(true);
	}
}

void Client::uiFocusOnOverviewTab() {
	m_ui->tabWidget->setCurrentWidget(m_ui->tabOverview);
}

void Client::listContactsOnDoubleClick(QListWidgetItem* item) {
	if ((!m_databaseWrapper.hasDatabase()) || (!m_messageCenterWrapper.hasMessageCenter()) || (m_tabController == nullptr)) {
		return;
	}

	openmittsu::widgets::ContactListWidgetItem* clwi = dynamic_cast<openmittsu::widgets::ContactListWidgetItem*>(item);
	if (clwi != nullptr) {
		openmittsu::protocol::ContactId const contactId = clwi->getContactId();
		if (!m_tabController->hasTab(contactId)) {
			openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
			m_tabController->openTab(contactId, pool.getBackedContact(contactId, m_databaseWrapper, m_messageCenterWrapper));
		}
		m_tabController->focusTab(contactId);
	} else {
		LOGGER()->warn("Could not determine the type of element the user double clicked on in the contacts list.");
		return;
	}
}

void Client::listGroupsOnDoubleClick(QListWidgetItem* item) {
	if ((!m_databaseWrapper.hasDatabase()) || (!m_messageCenterWrapper.hasMessageCenter()) || (m_tabController == nullptr)) {
		return;
	}

	openmittsu::widgets::GroupListWidgetItem* glwi = dynamic_cast<openmittsu::widgets::GroupListWidgetItem*>(item);
	if (glwi != nullptr) {
		openmittsu::protocol::GroupId const groupId = glwi->getGroupId();
		if (!m_tabController->hasTab(groupId)) {
			openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
			m_tabController->openTab(groupId, pool.getBackedGroup(groupId, m_databaseWrapper, m_messageCenterWrapper));
		}
		m_tabController->focusTab(groupId);
	} else {
		LOGGER()->warn("Could not determine the type of element the user double clicked on in the contacts list.");
		return;
	}
}

void Client::listContactsOnContextMenu(QPoint const& pos) {
	if ((!m_databaseWrapper.hasDatabase()) || (!m_messageCenterWrapper.hasMessageCenter()) || (m_tabController == nullptr)) {
		return;
	}
	QPoint globalPos = m_ui->listContacts->viewport()->mapToGlobal(pos);
	QListWidgetItem* listItem = m_ui->listContacts->itemAt(pos);
	openmittsu::widgets::ContactListWidgetItem* clwi = dynamic_cast<openmittsu::widgets::ContactListWidgetItem*>(listItem);

	if (clwi != nullptr) {
		QMenu listContactsContextMenu;

		QAction* actionHeadline = nullptr;
		QAction* actionEdit = nullptr;
		QAction* actionOpenClose = nullptr;

		bool isChatWindowOpen = false;
		if (clwi != nullptr) {
			actionHeadline = new QAction(QString(tr("Identity: %1")).arg(clwi->getContactId().toQString()), &listContactsContextMenu);
			listContactsContextMenu.addAction(actionHeadline);
			actionEdit = new QAction(tr("Edit Contact"), &listContactsContextMenu);
			listContactsContextMenu.addAction(actionEdit);

			isChatWindowOpen = m_tabController->hasTab(clwi->getContactId());
			if (isChatWindowOpen) {
				actionOpenClose = new QAction(tr("Close Chat Window"), &listContactsContextMenu);
			} else {
				actionOpenClose = new QAction(tr("Open Chat Window"), &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionOpenClose);

			QAction* separator = new QAction(&listContactsContextMenu);
			separator->setSeparator(true);
			listContactsContextMenu.addAction(separator);

			QString statusText;
			openmittsu::database::ContactData const contactData = m_databaseWrapper.getContactData(clwi->getContactId(), true);

			openmittsu::protocol::AccountStatus const status = contactData.accountStatus;
			if (status == openmittsu::protocol::AccountStatus::STATUS_ACTIVE) {
				statusText = tr("active");
			} else if (status == openmittsu::protocol::AccountStatus::STATUS_UNKNOWN) {
				statusText = tr("unknown");
			} else if (status == openmittsu::protocol::AccountStatus::STATUS_INACTIVE) {
				statusText = tr("inactive");
			} else {
				statusText = tr("invalid");
			}
			QAction* contactStatus = new QAction(QString(tr("Status: %1")).arg(statusText), &listContactsContextMenu);
			contactStatus->setDisabled(true);
			listContactsContextMenu.addAction(contactStatus);

			QAction* messageCount = new QAction(QString(tr("Stored Messages: %1")).arg(contactData.messageCount), &listContactsContextMenu);
			messageCount->setDisabled(true);
			listContactsContextMenu.addAction(messageCount);

			QHash<openmittsu::protocol::GroupId, QString> const groups = m_databaseWrapper.getKnownGroupsContainingMember(clwi->getContactId());
			if (groups.size() < 1) {
				QAction* groupMembership = new QAction(tr("Not a member of any known group"), &listContactsContextMenu);
				groupMembership->setDisabled(true);
				listContactsContextMenu.addAction(groupMembership);
			} else {
				QAction* groupMembership = new QAction(tr("Group memberships:"), &listContactsContextMenu);
				groupMembership->setDisabled(true);
				listContactsContextMenu.addAction(groupMembership);

				auto it = groups.constBegin();
				auto const end = groups.constEnd();
				for (; it != end; ++it) {
					QAction* groupMember = new QAction(QString(" - ").append(*it), &listContactsContextMenu);
					groupMember->setDisabled(true);
					listContactsContextMenu.addAction(groupMember);
				}
			}
		}

		QAction* selectedItem = listContactsContextMenu.exec(globalPos);
		if (selectedItem != nullptr) {
			if (selectedItem == actionEdit) {
				openmittsu::database::ContactData contactData = m_databaseWrapper.getContactData(clwi->getContactId(), false);
				QString const id = clwi->getContactId().toQString();
				QString const pubKey = contactData.publicKey.toQString();
				QString const nickname = contactData.nickName;

				openmittsu::dialogs::ContactEditDialog contactEditDialog(id, pubKey, nickname, this);

				int result = contactEditDialog.exec();

				if (result == QDialog::DialogCode::Accepted) {
					QString const newNickname = contactEditDialog.getNickname();
					if (nickname != newNickname) {
						m_databaseWrapper.setContactNickName(clwi->getContactId(), newNickname);
					}
				}
			} else if (selectedItem == actionOpenClose) {
				if (isChatWindowOpen) {
					m_tabController->closeTab(clwi->getContactId());
				} else {
					if (!m_tabController->hasTab(clwi->getContactId())) {
						openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
						m_tabController->openTab(clwi->getContactId(), pool.getBackedContact(clwi->getContactId(), m_databaseWrapper, m_messageCenterWrapper));
					}
					m_tabController->focusTab(clwi->getContactId());
				}
			}
		}
	}
}

void Client::listGroupsOnContextMenu(QPoint const& pos) {
	if ((!m_databaseWrapper.hasDatabase()) || (!m_messageCenterWrapper.hasMessageCenter()) || (m_tabController == nullptr)) {
		return;
	}

	QPoint globalPos = m_ui->listGroups->viewport()->mapToGlobal(pos);
	QListWidgetItem* listItem = m_ui->listGroups->itemAt(pos);
	openmittsu::widgets::GroupListWidgetItem* glwi = dynamic_cast<openmittsu::widgets::GroupListWidgetItem*>(listItem);

	if (glwi != nullptr) {
		QMenu listGroupsContextMenu;

		QAction* actionHeadline = nullptr;
		QAction* actionEdit = nullptr;
		QAction* actionOpenClose = nullptr;
		QAction* actionRequestSync = nullptr;

		bool isChatWindowOpen = false;
		bool isGroupSelfOwned = false;
		if (glwi != nullptr) {
			actionHeadline = new QAction(QString(tr("Group: %1")).arg(glwi->getGroupId().toQString()), &listGroupsContextMenu);
			listGroupsContextMenu.addAction(actionHeadline);
			actionEdit = new QAction("Edit Group", &listGroupsContextMenu);
			listGroupsContextMenu.addAction(actionEdit);

			isChatWindowOpen = m_tabController->hasTab(glwi->getGroupId());
			if (isChatWindowOpen) {
				actionOpenClose = new QAction(tr("Close Chat Window"), &listGroupsContextMenu);
			} else {
				actionOpenClose = new QAction(tr("Open Chat Window"), &listGroupsContextMenu);
			}
			listGroupsContextMenu.addAction(actionOpenClose);

			if (glwi->getGroupId().getOwner() == m_databaseWrapper.getSelfContact()) {
				isGroupSelfOwned = true;
				actionRequestSync = new QAction(tr("Force Group Sync"), &listGroupsContextMenu);
			} else {
				actionRequestSync = new QAction(tr("Request Group Sync"), &listGroupsContextMenu);
			}
			listGroupsContextMenu.addAction(actionRequestSync);
			if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected()) {
				actionRequestSync->setDisabled(true);
			}

			QAction* separator = new QAction(&listGroupsContextMenu);
			separator->setSeparator(true);
			listGroupsContextMenu.addAction(separator);

			openmittsu::database::GroupData const groupData = m_databaseWrapper.getGroupData(glwi->getGroupId(), false);
			QAction* messageCount = new QAction(QString(tr("Stored Messages: %1")).arg(groupData.messageCount), &listGroupsContextMenu);
			messageCount->setDisabled(true);
			listGroupsContextMenu.addAction(messageCount);

			QAction* groupMembers = new QAction(tr("Group members:"), &listGroupsContextMenu);
			groupMembers->setDisabled(true);
			listGroupsContextMenu.addAction(groupMembers);

			//QSet<openmittsu::protocol::ContactId> const members = m_databaseWrapper.getGroupMembers(glwi->getGroupId(), false);
			QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> contactData = m_databaseWrapper.getContactDataAll(false);

			for (openmittsu::protocol::ContactId const& member : groupData.members) {
				QAction* groupMember = new QAction(QString(" - ").append(contactData.constFind(member)->nickName), &listGroupsContextMenu);
				groupMember->setDisabled(true);
				listGroupsContextMenu.addAction(groupMember);
			}
		}

		QAction* selectedItem = listGroupsContextMenu.exec(globalPos);
		if (selectedItem != nullptr) {
			if (selectedItem == actionEdit) {
				showNotYetImplementedInfo();
			} else if (selectedItem == actionOpenClose) {
				if (isChatWindowOpen) {
					m_tabController->closeTab(glwi->getGroupId());
				} else {
					if (!m_tabController->hasTab(glwi->getGroupId())) {
						openmittsu::dataproviders::BackedContactAndGroupPool& pool = openmittsu::dataproviders::BackedContactAndGroupPool::getInstance();
						m_tabController->openTab(glwi->getGroupId(), pool.getBackedGroup(glwi->getGroupId(), m_databaseWrapper, m_messageCenterWrapper));
					}
					m_tabController->focusTab(glwi->getGroupId());
				}
			} else if ((selectedItem == actionRequestSync) && (actionRequestSync != nullptr)) {
				if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected() || (!m_messageCenterWrapper.hasMessageCenter())) {
					return;
				}

				if (isGroupSelfOwned) {
					m_messageCenterWrapper.resendGroupSetup(glwi->getGroupId());
				} else {
					m_messageCenterWrapper.sendSyncRequest(glwi->getGroupId());
				}
			}
		}
	}
}

/*
	MENU ENTRIES & HANDLING
*/

void Client::menuFileOptionsOnClick() {
	if (!m_databaseWrapper.hasDatabase() || m_optionMaster == nullptr) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		openmittsu::dialogs::OptionsDialog optionsDialog(m_optionMaster, this);
		optionsDialog.exec();
	}
}

void Client::menuFileShowFirstUseWizardOnClick() {
	openmittsu::wizards::FirstUseWizard firstUseWizard(this);
	int const result = firstUseWizard.exec();
	if (result == 1) {
		openmittsu::wizards::FirstUseWizard::UserChoice const userChoice = firstUseWizard.getUserChoice();
		if (userChoice == openmittsu::wizards::FirstUseWizard::UserChoice::LOAD_DATABASE) {
			btnOpenDatabaseOnClick();
		} else if (userChoice == openmittsu::wizards::FirstUseWizard::UserChoice::CREATE_DATABASE) {
			menuIdentityLoadBackupOnClick();
		}
	}
}

void Client::menuFileExitOnClick() {
	this->close();
}

void Client::menuAboutLicenseOnClick() {
	openmittsu::dialogs::LicenseDialog licenseDialog(this);
	licenseDialog.exec();
}

void Client::menuAboutAboutOnClick() {
	QMessageBox::about(this, "OpenMittsu - About", QString("<h2>OpenMittsu</h2><br><br>%1<br>%2<br><br>An open source chat client for Threema-style end-to-end encrypted chat networks.<br><br>This project is in no way connected, affiliated or endorsed with/by Threema GmbH.<br><br>Copyright (C) 2015-19 by Philipp Berger<br>This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.<br>See LICENSE for further information.<br><br>Don't be a jerk!").arg(QString::fromStdString(openmittsu::utility::Version::longVersionString())).arg(QString::fromStdString(openmittsu::utility::Version::buildInfo())));
}

void Client::menuAboutAboutQtOnClick() {
	QMessageBox::aboutQt(this, "About Qt");
}

void Client::menuGroupAddOnClick() {
	if ((!m_databaseWrapper.hasDatabase()) || (!m_messageCenterWrapper.hasMessageCenter())) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected()) {
		QMessageBox::warning(this, "Not connected to a server", "Before you can use this feature you need to connect to a server.");
	} else {
		QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> contactData = m_databaseWrapper.getContactDataAll(false);
		contactData.remove(m_databaseWrapper.getSelfContact());

		openmittsu::wizards::GroupCreationWizard groupCreationWizard(contactData, std::make_unique<openmittsu::dataproviders::SimpleGroupCreationProcessor>(m_messageCenterWrapper), this);
		groupCreationWizard.exec();
	}
}

void Client::menuAboutStatisticsOnClick() {
	if ((m_protocolClient == nullptr) || (!m_protocolClient->getIsConnected())) {
		QMessageBox::warning(this, "OpenMittsu - Statistics", "Not connected, can not show session statistics.");
	} else {
		QDateTime now = QDateTime::currentDateTime();
		quint64 seconds = m_protocolClient->getConnectedSince().secsTo(now);
		QMessageBox::information(this, "OpenMittsu - Statistics", QString("Current session:\n\nTime connected: %1\nSend: %2 Bytes\nReceived: %3 Bytes\nMessages send: %4\nMessages received: %5").arg(formatDuration(seconds)).arg(QString::number(m_protocolClient->getSendBytesCount(), 10)).arg(QString::number(m_protocolClient->getReceivedBytesCount(), 10)).arg(QString::number(m_protocolClient->getSendMessagesCount(), 10)).arg(QString::number(m_protocolClient->getReceivedMessagesCount(), 10)));
	}
}

void Client::menuViewShowEmojiTabOnTriggered(bool checked) {
	//
}

void Client::menuGroupEditOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuGroupLeaveOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuContactAddOnClick() {
	if (!m_databaseWrapper.hasDatabase()) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
		return;
	}

	openmittsu::dialogs::ContactAddDialog contactAddDialog(this);
	int result = contactAddDialog.exec();

	if (result == QDialog::DialogCode::Accepted) {
		QString const identityString = contactAddDialog.getIdentity();
		QString const nickname = contactAddDialog.getNickname();
		if (identityString.isEmpty() || (identityString.size() != 8)) {
			QMessageBox::warning(this, "Could not add Contact", "The identity entered is not well formed.\nNo contact has been added.");
		} else if (m_databaseWrapper.hasContact(openmittsu::protocol::ContactId(identityString.toUtf8()))) {
			QMessageBox::warning(this, "Could not add Contact", "The identity entered is already known.\nThe contact has not been changed.");
		} else {
			try {
				openmittsu::protocol::ContactId const contactId(identityString.toUtf8());
				openmittsu::tasks::IdentityReceiverCallbackTask ir(m_serverConfiguration, contactId);

				QEventLoop eventLoop;
				OPENMITTSU_CONNECT(&ir, finished(openmittsu::tasks::CallbackTask*), &eventLoop, quit());
				ir.start();
				eventLoop.exec();

				if (!ir.hasFinished() || !ir.hasFinishedSuccessfully()) {
					QMessageBox::warning(this, "Could not add Contact", QString("Error while downloading public-key from identity servers: %1\nNo Contact has been added.").arg(ir.getErrorMessage()));
				} else if (ir.getContactIdOfFetchedPublicKey() != contactId) {
					QMessageBox::warning(this, "Could not add Contact", QString("Error while downloading public-key from identity servers: Internal Error: Missmatch between requested and received contact data.\nNo Contact has been added."));
				} else {
					m_databaseWrapper.storeNewContact(contactId, ir.getFetchedPublicKey());
					if (!nickname.isEmpty()) {
						m_databaseWrapper.setContactNickName(contactId, nickname);
					}

					QMessageBox::information(this, "Contact added", QString("Contact successfully added!\nIdentity: %1\nPublic Key: %2\n").arg(contactId.toQString()).arg(ir.getFetchedPublicKey().toQString()));
				}
			} catch (openmittsu::exceptions::ProtocolErrorExceptionImpl& pee) {
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
	if (!m_databaseWrapper.hasDatabase()) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		std::shared_ptr<openmittsu::backup::IdentityBackup> const backupData = m_databaseWrapper.getBackup();
		openmittsu::dialogs::FingerprintDialog fingerprintDialog(backupData->getClientContactId(), backupData->getClientLongTermKeyPair(), this);
		fingerprintDialog.exec();
	}
}

void Client::menuIdentityShowPublicKeyOnClick() {
	if (!m_databaseWrapper.hasDatabase()) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		std::shared_ptr<openmittsu::backup::IdentityBackup> const backupData = m_databaseWrapper.getBackup();
		openmittsu::dialogs::ShowIdentityAndPublicKeyDialog showIdentityAndPublicKeyDialog(backupData->getClientContactId(), backupData->getClientLongTermKeyPair(), this);
		showIdentityAndPublicKeyDialog.exec();
	}
}

void Client::menuIdentityCreateBackupOnClick() {
	if (!m_databaseWrapper.hasDatabase()) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		openmittsu::wizards::BackupCreationWizard backupCreationWizard(*m_databaseWrapper.getBackup(), this);
		backupCreationWizard.exec();
	}
}

void Client::menuIdentityLoadBackupOnClick(QString const& legacyClientConfigurationFileName) {
	openmittsu::wizards::LoadBackupWizard loadBackupWizard(legacyClientConfigurationFileName, this);
	int const result = loadBackupWizard.exec();
	if (result == 1) {
		QString const dbFileName = loadBackupWizard.getDatabaseFileName();
		if (!dbFileName.isEmpty() && !dbFileName.isNull()) {
			openDatabaseFile(dbFileName);
		}
	}
}

void Client::menuDatabaseImportLegacyContactsAndGroupsOnClick(QString const& legacyContactsFileName) {
	if (!m_databaseWrapper.hasDatabase()) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		QString fileName;
		if (!legacyContactsFileName.isNull() && !legacyContactsFileName.isEmpty()) {
			fileName = legacyContactsFileName;
		} else {
			fileName = QFileDialog::getOpenFileName(this, tr("Select the legacy contacts and group file"));
			if (fileName.isNull() || fileName.isEmpty()) {
				return;
			}
		}

		try {
			openmittsu::utility::LegacyContactImporter lci(openmittsu::utility::LegacyContactImporter::fromFile(fileName));

			{
				QVector<openmittsu::database::NewContactData> newContacts;
				QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& contacts = lci.getContacts();
				auto it = contacts.constBegin();
				auto end = contacts.constEnd();
				for (; it != end; ++it) {
					openmittsu::database::NewContactData newContact;
					newContact.id = it.key();
					newContact.publicKey = it.value().first;
					newContact.verificationStatus = openmittsu::protocol::ContactIdVerificationStatus::VERIFICATION_STATUS_SERVER_VERIFIED;
					newContact.nickName = it.value().second;

					newContacts.append(newContact);
				}
				m_databaseWrapper.storeNewContact(newContacts);
			}

			{
				QVector<openmittsu::database::NewGroupData> newGroups;
				QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& groups = lci.getGroups();
				auto it = groups.constBegin();
				auto end = groups.constEnd();
				for (; it != end; ++it) {
					openmittsu::database::NewGroupData newGroup;
					newGroup.id = it.key();
					newGroup.members = it.value().first;
					newGroup.name = it.value().second;
					newGroup.createdAt = openmittsu::protocol::MessageTime::now();
					newGroup.isAwaitingSync = false;
					newGroup.isDeleted = false;

					newGroups.append(newGroup);
				}
				m_databaseWrapper.storeNewGroup(newGroups);
			}

			contactRegistryOnIdentitiesChanged();

			QMessageBox::information(this, tr("Success!"), tr("Successfully imported %1 legacy contacts and %2 legacy groups.").arg(lci.getContactCount()).arg(lci.getGroupCount()));
		} catch (...) {
			QMessageBox::warning(this, tr("Could not import legacy contact file"), tr("Could not import legacy contact and group file!"));
		}
	}
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
	if ((m_protocolClient != nullptr) && (m_protocolClient->getIsConnected())) {
		QString const text(QStringLiteral("Connected (since %1)"));
		QDateTime const now = QDateTime::currentDateTime();
		quint64 const seconds = m_protocolClient->getConnectedSince().secsTo(now);

		m_ui->lblStatus->setText(text.arg(formatDuration(seconds)));
	} else {
		QString const text(QStringLiteral("Not connected"));
		m_ui->lblStatus->setText(text);
	}
}

void Client::unreadMessagesIconBlinkTimerOnTimer() {
	if (QApplication::activeWindow() != nullptr) {
		// deactivate timer as window is active
		setAppIcon(false);
		m_unreadMessagesIconBlinkTimer.stop();
		m_unreadMessagesIconBlinkState = false;
	} else {
		if (m_unreadMessagesIconBlinkState) {
			setAppIcon(false);
		} else {
			setAppIcon(true);
		}
		m_unreadMessagesIconBlinkState = !m_unreadMessagesIconBlinkState;
	}
}

void Client::setAppIcon(bool haveUnreadMessages) {
	if (haveUnreadMessages) {
		QIcon icon(":/icons/logo_unread_message.ico");
		this->setWindowIcon(icon);
	} else {
		QIcon icon(":/icons/logo.ico");
		this->setWindowIcon(icon);
	}
}

void Client::showNotYetImplementedInfo() {
	QMessageBox::information(this, "Not yet implemented!", "Sorry!\nThis feature is not yet implemented.");
}

void Client::callbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask) {
	if (callbackTask == nullptr) {
		LOGGER()->error("A null callback task finished.");
		return;
	}

	if (dynamic_cast<openmittsu::tasks::CheckFeatureLevelCallbackTask*>(callbackTask) != nullptr) {
		openmittsu::utility::UniquePtrWithDelayedThreadDeletion<openmittsu::tasks::CheckFeatureLevelCallbackTask> checkFeatureLevelTask(dynamic_cast<openmittsu::tasks::CheckFeatureLevelCallbackTask*>(callbackTask));
		if (checkFeatureLevelTask->hasFinishedSuccessfully()) {
			openmittsu::protocol::ContactId const selfIdentity = (!m_databaseWrapper.hasDatabase()) ? openmittsu::protocol::ContactId(0) : m_databaseWrapper.getSelfContact();

			if (m_databaseWrapper.hasDatabase()) {
				m_databaseWrapper.setContactFeatureLevelBatch(checkFeatureLevelTask->getFetchedFeatureLevels());
			}
			QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> result = checkFeatureLevelTask->getFetchedFeatureLevels();
			QHashIterator<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> i(result);
			while (i.hasNext()) {
				i.next();
				LOGGER_DEBUG("Contact {} has feature level {}.", i.key().toString(), openmittsu::protocol::FeatureLevelHelper::toInt(i.value()));
			}

			if (result.contains(selfIdentity)) {
				openmittsu::protocol::FeatureLevel const selfFeatureLevel = result.value(selfIdentity);
				openmittsu::protocol::FeatureLevel const openMittsuFeatureLevel = openmittsu::protocol::FeatureLevelHelper::latestSupported();

				if (openmittsu::protocol::FeatureLevelHelper::lessThan(selfFeatureLevel, openMittsuFeatureLevel)) {
					if ((!m_databaseWrapper.hasDatabase()) || (m_serverConfiguration == nullptr)) {
						LOGGER()->error("Wanted to update feature level, but either database or server config is null!");
						return;
					}

					std::shared_ptr<openmittsu::backup::IdentityBackup> const backupData = m_databaseWrapper.getBackup();
					openmittsu::crypto::BasicCryptoBox basicCryptoBox(backupData->getClientLongTermKeyPair(), m_serverConfiguration->getServerLongTermPublicKey());
					openmittsu::tasks::SetFeatureLevelCallbackTask* setFeatureLevelTask = new openmittsu::tasks::SetFeatureLevelCallbackTask(m_serverConfiguration, basicCryptoBox, backupData->getClientContactId(), openMittsuFeatureLevel);
					OPENMITTSU_CONNECT(setFeatureLevelTask, finished(openmittsu::tasks::CallbackTask*), this, callbackTaskFinished(openmittsu::tasks::CallbackTask*));
					setFeatureLevelTask->start();
				}
			}
		} else {
			LOGGER()->error("Checking for supported feature levels of contacts failed: {}", checkFeatureLevelTask->getErrorMessage().toStdString());
		}
	} else if (dynamic_cast<openmittsu::tasks::CheckContactActivityStatusCallbackTask*>(callbackTask) != nullptr) {
		openmittsu::utility::UniquePtrWithDelayedThreadDeletion<openmittsu::tasks::CheckContactActivityStatusCallbackTask> checkContactIdStatusTask(dynamic_cast<openmittsu::tasks::CheckContactActivityStatusCallbackTask*>(callbackTask));
		if (checkContactIdStatusTask->hasFinishedSuccessfully()) {
			if (!m_databaseWrapper.hasDatabase()) {
				LOGGER()->error("Wanted to update feature levels, but the database is null!");
				return;
			}

			m_databaseWrapper.setContactAccountStatusBatch(checkContactIdStatusTask->getFetchedStatus());
		} else {
			LOGGER()->error("Checking for status of contacts failed: {}", checkContactIdStatusTask->getErrorMessage().toStdString());
		}
	} else if (dynamic_cast<openmittsu::tasks::SetFeatureLevelCallbackTask*>(callbackTask) != nullptr) {
		openmittsu::utility::UniquePtrWithDelayedThreadDeletion<openmittsu::tasks::SetFeatureLevelCallbackTask> setFeatureLevelTask(dynamic_cast<openmittsu::tasks::SetFeatureLevelCallbackTask*>(callbackTask));
		if (setFeatureLevelTask->hasFinishedSuccessfully()) {
			LOGGER()->info("Updated feature level for used Client ID to latest support version {}.", openmittsu::protocol::FeatureLevelHelper::toInt(openmittsu::protocol::FeatureLevelHelper::latestSupported()));
		} else {
			LOGGER()->error("Updating the feature level for used Client ID to latest support version {} failed: {}", openmittsu::protocol::FeatureLevelHelper::toInt(openmittsu::protocol::FeatureLevelHelper::latestSupported()), setFeatureLevelTask->getErrorMessage().toStdString());
		}
	}
}
