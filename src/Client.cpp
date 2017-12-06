#include <QtWidgets>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "Client.h"
#include "src/network/ServerConfiguration.h"

#include "ui_main.h"

#include "src/wizards/BackupCreationWizard.h"
#include "src/wizards/GroupCreationWizard.h"
#include "src/wizards/LoadBackupWizard.h"
#include "src/wizards/FirstUseWizard.h"

#include "src/dialogs/ShowIdentityAndPublicKeyDialog.h"
#include "src/dialogs/ContactAddDialog.h"
#include "src/dialogs/ContactEditDialog.h"
#include "src/dialogs/FingerprintDialog.h"
#include "src/dialogs/OptionsDialog.h"
#include "src/dialogs/UpdaterDialog.h"

#include "src/widgets/SimpleContactChatTab.h"
#include "src/widgets/SimpleGroupChatTab.h"
#include "src/widgets/SimpleTabController.h"
#include "src/widgets/ContactListWidgetItem.h"
#include "src/widgets/GroupListWidgetItem.h"
#include "src/widgets/LicenseDialog.h"

#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/Version.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/ThreadDeleter.h"
#include "src/utility/LegacyContactImporter.h"
#include "src/backup/BackupReader.h"

#include "src/backup/BackupReader.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InvalidPasswordOrDatabaseException.h"
#include "src/exceptions/ProtocolErrorException.h"

#include "src/tasks/IdentityReceiverCallbackTask.h"
#include "src/tasks/CheckFeatureLevelCallbackTask.h"
#include "src/tasks/CheckContactActivityStatusCallbackTask.h"
#include "src/tasks/SetFeatureLevelCallbackTask.h"

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include "src/crypto/FullCryptoBox.h"
#include "src/dataproviders/KeyRegistry.h"
#include "src/protocol/GroupRegistry.h"

#include "src/dataproviders/MessageCenter.h"
#include "src/dataproviders/SimpleGroupCreationProcessor.h"
#include "Config.h"

Client::Client(QWidget* parent) : QMainWindow(parent), 
	m_ui(), 
	m_protocolClient(nullptr), m_protocolClientThread(this), m_connectionTimer(this),
	m_updater(),
	m_connectionState(ConnectionState::STATE_DISCONNECTED),
	m_tabController(nullptr),
	m_messageCenter(nullptr),
	m_serverConfiguration(std::make_shared<openmittsu::network::ServerConfiguration>()),
	m_optionMaster(std::make_shared<openmittsu::utility::OptionMaster>()),
	m_database(nullptr),
	m_audioNotifier(std::make_shared<openmittsu::utility::AudioNotification>()) 

{
	m_ui.setupUi(this);

	m_tabController = std::make_shared<openmittsu::widgets::SimpleTabController>(m_ui.tabWidget);
	m_messageCenter = std::make_shared<openmittsu::dataproviders::MessageCenter>(m_tabController, m_optionMaster);


	m_ui.listContacts->setContextMenuPolicy(Qt::CustomContextMenu);
	m_connectionTimer.start(500);
	OPENMITTSU_CONNECT(&m_connectionTimer, timeout(), this, connectionTimerOnTimer());
	OPENMITTSU_CONNECT(m_messageCenter.get(), newUnreadMessageAvailable(openmittsu::widgets::ChatTab*), this, messageCenterOnHasUnreadMessages(openmittsu::widgets::ChatTab*));

	// Check whether QSqlCipher is available
	if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLCIPHER"))) {
#ifdef OPENMITTSU_CONFIG_ALLOW_MISSING_QSQLCIPHER
		QMessageBox::warning(this, tr("Database driver not available"), tr("openMittsu relies on SqlCipher and QSqlCipher for securely storing the database.\nThe QSQLCIPHER driver is not available. It should reside in the sqldrivers\\ subdirectory of openMittsu.\nWe will use the unencrypted SQLITE driver instead."));
#else
		QMessageBox::warning(this, tr("Database driver not available"), tr("openMittsu relies on SqlCipher and QSqlCipher for securely storing the database.\nThe QSQLCIPHER driver is not available. It should reside in the sqldrivers\\ subdirectory of openMittsu.\nSince no encryption is available, OpenMittsu will now terminate."));
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
	this->m_optionMaster = std::make_shared<openmittsu::utility::OptionMaster>();
	QString const databaseFile = m_optionMaster->getOptionAsQString(openmittsu::utility::OptionMaster::Options::FILEPATH_DATABASE);
	QString const legacyClientConfiguration = m_optionMaster->getOptionAsQString(openmittsu::utility::OptionMaster::Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION);
	bool showFirstUseWizard = false;
	bool showFromBackupWizard = false;

	if (!databaseFile.isEmpty()) {
		if (!QFile::exists(databaseFile)) {
			LOGGER_DEBUG("Removing key \"FILEPATH_DATABASE\" from stored settings as the file is not valid.");
			m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_DATABASE, "");
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

	OPENMITTSU_CONNECT(m_ui.btnConnect, clicked(), this, btnConnectOnClick());
	OPENMITTSU_CONNECT(m_ui.btnOpenDatabase, clicked(), this, btnOpenDatabaseOnClick());
	OPENMITTSU_CONNECT(m_ui.listContacts, itemDoubleClicked(QListWidgetItem*), this, listContactsOnDoubleClick(QListWidgetItem*));
	OPENMITTSU_CONNECT(m_ui.listContacts, customContextMenuRequested(const QPoint&), this, listContactsOnContextMenu(const QPoint&));

	// Menus
	OPENMITTSU_CONNECT(m_ui.actionLicense, triggered(), this, menuAboutLicenseOnClick());
	OPENMITTSU_CONNECT(m_ui.actionAbout, triggered(), this, menuAboutAboutOnClick());
	OPENMITTSU_CONNECT(m_ui.actionAbout_Qt, triggered(), this, menuAboutAboutQtOnClick());
	OPENMITTSU_CONNECT(m_ui.actionAdd_a_Contact, triggered(), this, menuContactAddOnClick());
	OPENMITTSU_CONNECT(m_ui.actionDelete_a_Contact, triggered(), this, menuContactDeleteOnClick());
	OPENMITTSU_CONNECT(m_ui.actionEdit_a_Contact, triggered(), this, menuContactEditOnClick());
	OPENMITTSU_CONNECT(m_ui.actionSave_to_file, triggered(), this, menuContactSaveToFileOnClick());
	OPENMITTSU_CONNECT(m_ui.actionAdd_Group, triggered(), this, menuGroupAddOnClick());
	OPENMITTSU_CONNECT(m_ui.actionEdit_Group, triggered(), this, menuGroupEditOnClick());
	OPENMITTSU_CONNECT(m_ui.actionLeave_Group, triggered(), this, menuGroupLeaveOnClick());
	OPENMITTSU_CONNECT(m_ui.actionCreate_Backup, triggered(), this, menuIdentityCreateBackupOnClick());
	OPENMITTSU_CONNECT(m_ui.actionLoad_Backup, triggered(), this, menuIdentityLoadBackupOnClick());
	OPENMITTSU_CONNECT(m_ui.actionShow_Fingerprint, triggered(), this, menuIdentityShowFingerprintOnClick());
	OPENMITTSU_CONNECT(m_ui.actionShow_Public_Key, triggered(), this, menuIdentityShowPublicKeyOnClick());
	OPENMITTSU_CONNECT(m_ui.actionImport_legacy_contacts_and_groups, triggered(), this, menuDatabaseImportLegacyContactsAndGroupsOnClick());
	OPENMITTSU_CONNECT(m_ui.actionStatistics, triggered(), this, menuAboutStatisticsOnClick());
	OPENMITTSU_CONNECT(m_ui.actionOptions, triggered(), this, menuFileOptionsOnClick());
	OPENMITTSU_CONNECT(m_ui.actionShow_First_Use_Wizard, triggered(), this, menuFileShowFirstUseWizardOnClick());
	OPENMITTSU_CONNECT(m_ui.actionExit, triggered(), this, menuFileExitOnClick());

	m_protocolClientThread.start();
	OPENMITTSU_CONNECT(&m_protocolClientThread, finished(), this, threadFinished());

#ifndef OPENMITTSU_CONFIG_DISABLE_VERSION_UPDATE_CHECK
	// Call Updater
	OPENMITTSU_CONNECT_QUEUED(&m_updater, foundNewVersion(int, int, int, int, QString, QString, QString), this, updaterFoundNewVersion(int, int, int, int, QString, QString, QString));
	QTimer::singleShot(0, &m_updater, SLOT(start()));
#endif

	contactRegistryOnIdentitiesChanged();

	// Restore Window location and size
	restoreGeometry(m_optionMaster->getOptionAsQByteArray(openmittsu::utility::OptionMaster::Options::BINARY_MAINWINDOW_GEOMETRY));
	restoreState(m_optionMaster->getOptionAsQByteArray(openmittsu::utility::OptionMaster::Options::BINARY_MAINWINDOW_STATE));

	if (showFirstUseWizard) {
		menuFileShowFirstUseWizardOnClick();
	} else if (showFromBackupWizard) {
		auto const resultButton = QMessageBox::question(this, tr("Legacy client configuration found"), tr("Welcome.\nIt seems that you used an older version of openMittsu before that used a plaintext client configuration file to store your ID.\nThis has been replaced by an encrypted database storing both your ID, contacts, groups and messages.\nIf you want, your legacy ID file can be converted into a modern openMittsu database. Click yes to import the old file."));
		if (resultButton == QMessageBox::StandardButton::Yes) {
			menuIdentityLoadBackupOnClick(legacyClientConfiguration);
			m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_LEGACY_CLIENT_CONFIGURATION, "");
		}
	}

	if (m_database) {
		QString const legacyContactsFile = m_optionMaster->getOptionAsQString(openmittsu::utility::OptionMaster::Options::FILEPATH_LEGACY_CONTACTS_DATABASE);
		if (!legacyContactsFile.isEmpty() && QFile::exists(legacyContactsFile)) {
			auto const resultButton = QMessageBox::question(this, tr("Legacy contacts file found"), tr("Welcome.\nIt seems that you used an older version of openMittsu before that used a plaintext contacts file to store your contacts and groups.\nThis has been replaced by an encrypted database storing both your ID, contacts, groups and messages.\nIf you want, your legacy contacts file can be imported into your openMittsu database.\nClick \"Yes\" to import the old file (located at %1), or \"No\" to leave it for now.\nYou can always come back later and import it via Database -> Import openMittsu....").arg(legacyContactsFile));
			if (resultButton == QMessageBox::StandardButton::Yes) {
				menuDatabaseImportLegacyContactsAndGroupsOnClick(legacyContactsFile);
				m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_LEGACY_CONTACTS_DATABASE, "");
			}
		}
	}
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

void Client::closeEvent(QCloseEvent* event) {
	// Save location and size of window
	m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::BINARY_MAINWINDOW_GEOMETRY, saveGeometry());
	m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::BINARY_MAINWINDOW_STATE, saveState());
	
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

		this->m_messageCenter->setNetworkSentMessageAcceptor(nullptr);
		if (m_protocolClient->getIsConnected()) {
			m_protocolClient->disconnectFromServer();
		}

		QEventLoop eventLoop;
		OPENMITTSU_CONNECT(m_protocolClient.get(), teardownComplete(), &eventLoop, quit());
		QMetaObject::invokeMethod(m_protocolClient.get(), "teardown", Qt::QueuedConnection);

		m_protocolClient.reset();
	}

	QString const nickname = m_database->getContactNickname(m_database->getSelfContact());
	std::shared_ptr<openmittsu::crypto::FullCryptoBox> cryptoBox = std::make_shared<openmittsu::crypto::FullCryptoBox>(openmittsu::dataproviders::KeyRegistry(m_serverConfiguration->getServerLongTermPublicKey(), m_database));
	if (nickname.compare(QStringLiteral("You"), Qt::CaseInsensitive) == 0) {
		LOGGER()->info("Using only ID as PushFromID token (for iOS Push Receivers).");
		m_protocolClient = std::make_unique<openmittsu::network::ProtocolClient>(cryptoBox, m_database->getSelfContact(), m_serverConfiguration, m_optionMaster, std::make_shared<openmittsu::network::MessageCenterWrapper>(m_messageCenter), openmittsu::protocol::PushFromId(m_database->getSelfContact()));
	} else {
		m_protocolClient = std::make_unique<openmittsu::network::ProtocolClient>(cryptoBox, m_database->getSelfContact(), m_serverConfiguration, m_optionMaster, std::make_shared<openmittsu::network::MessageCenterWrapper>(m_messageCenter), openmittsu::protocol::PushFromId(nickname));
		LOGGER()->info("Using nickname \"{}\" as PushFromID token (for iOS Push Receivers).", nickname.toStdString());
	}

	m_protocolClient->moveToThread(&m_protocolClientThread);

	OPENMITTSU_CONNECT(m_protocolClient.get(), connectToFinished(int, QString), this, protocolClientOnConnectToFinished(int, QString));
	OPENMITTSU_CONNECT(m_protocolClient.get(), readyConnect(), this, protocolClientOnReadyConnect());
	OPENMITTSU_CONNECT(m_protocolClient.get(), lostConnection(), this, protocolClientOnLostConnection());
	OPENMITTSU_CONNECT(m_protocolClient.get(), duplicateIdUsageDetected(), this, protocolClientOnDuplicateIdUsageDetected());

	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(m_protocolClient.get(), setupDone(), &eventLoop, quit());

	QMetaObject::invokeMethod(m_protocolClient.get(), "setup", Qt::QueuedConnection);
	eventLoop.exec(); // blocks until "finished()" has been called

	m_messageCenter->setNetworkSentMessageAcceptor(std::make_shared<openmittsu::dataproviders::NetworkSentMessageAcceptor>(m_protocolClient));
}

void Client::threadFinished() {
	LOGGER_DEBUG("Client::threadFinished - the worker thread finished.");
	if (m_protocolClient != nullptr) {
		QMetaObject::invokeMethod(m_protocolClient.get(), "teardown", Qt::QueuedConnection);
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
	m_ui.lblDatabase->setText(currentFileName);
	if (m_database) {
		m_messageCenter->setStorage(m_database);
		m_optionMaster->setDatabase(m_database);

		m_database->enableTimers();
	}
}

void Client::btnConnectOnClick() {
	if (m_connectionState == ConnectionState::STATE_DISCONNECTED) {
		if ((m_serverConfiguration == nullptr) || (m_database == nullptr)) {
			QMessageBox::warning(this, "Can not connect", "Please choose a valid database file first.");
			return;
		}

		m_ui.btnConnect->setEnabled(false);
		m_ui.btnConnect->setText(tr("Connecting..."));

		setupProtocolClient();
		m_messageCenter->setStorage(m_database);
		QTimer::singleShot(0, m_protocolClient.get(), SLOT(connectToServer()));
	} else if (m_connectionState == ConnectionState::STATE_CONNECTING) {
		// No click should be possible in this state
		m_ui.btnConnect->setEnabled(false);
	} else if (m_connectionState == ConnectionState::STATE_CONNECTED) {
		m_ui.btnConnect->setEnabled(false);
		m_ui.btnConnect->setText(tr("Disconnecting..."));
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
		
		openmittsu::database::Database db(databaseFileName, password, folder);
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
		bool ok = false;
		QString const password = QInputDialog::getText(this, tr("Database password"), tr("Please enter the database password for file \"%1\":").arg(fileName), QLineEdit::Password, QString(), &ok);
		if (ok && !password.isNull()) {
			try {
				QDir location(fileName);
				location.cdUp();

				this->m_database = std::make_shared<openmittsu::database::Database>(fileName, password, location);
				this->m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_DATABASE, fileName);
				updateDatabaseInfo(fileName);

				contactRegistryOnIdentitiesChanged();
				break;
			} catch (openmittsu::exceptions::InternalErrorException&) {
				LOGGER_DEBUG("Removing key \"FILEPATH_DATABASE\" from stored settings as the file is not valid.");
				m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_DATABASE, "");
				break;
			} catch (openmittsu::exceptions::InvalidPasswordOrDatabaseException&) {
				QMessageBox::information(this, tr("Invalid password"), tr("The entered database password was invalid."));
			}
		} else {
			m_optionMaster->setOption(openmittsu::utility::OptionMaster::Options::FILEPATH_DATABASE, "");
			break;
		}
	}
}

void Client::contactRegistryOnIdentitiesChanged() {
	LOGGER_DEBUG("Updating contacts list on IdentitiesChanged() signal.");
	m_ui.listContacts->clear();
	
	if (m_database) {
		QHash<openmittsu::protocol::ContactId, QString> knownIdentities = m_database->getKnownContactsWithNicknames();
		auto it = knownIdentities.constBegin();
		auto const end = knownIdentities.constEnd();

		openmittsu::protocol::ContactId const selfIdentity = m_database->getSelfContact();
		for (; it != end; ++it) {
			openmittsu::protocol::ContactId const contactId = it.key();
			if (contactId == selfIdentity) {
				continue;
			}
			ContactListWidgetItem* clwi = nullptr;
			if (it->isNull() || it->isEmpty()) {
				clwi = new ContactListWidgetItem(contactId, false, contactId.toQString());
			} else {
				clwi = new ContactListWidgetItem(contactId, false, *it);
			}
			openmittsu::protocol::AccountStatus const status = m_database->getContactAccountStatus(contactId);
			if (status == openmittsu::protocol::AccountStatus::STATUS_INACTIVE) {
				clwi->setBackgroundColor(QColor::fromRgb(255, 255, 51));
			} else if (status == openmittsu::protocol::AccountStatus::STATUS_INVALID) {
				clwi->setBackgroundColor(QColor::fromRgb(250, 128, 114));
			}

			bool inserted = false;
			for (int i = 0; i < m_ui.listContacts->count(); ++i) {
				if (*clwi < *m_ui.listContacts->item(i)) {
					m_ui.listContacts->insertItem(i, clwi);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				m_ui.listContacts->addItem(clwi);
			}
		}

		// Groups
		QSet<openmittsu::protocol::GroupId> knownGroups = m_database->getKnownGroups();
		auto itGroups = knownGroups.constBegin();
		auto const endGroups = knownGroups.constEnd();

		for (; itGroups != endGroups; ++itGroups) {
			openmittsu::protocol::GroupId const groupId = *itGroups;
			GroupListWidgetItem* glwi = new GroupListWidgetItem(groupId, false, m_database->getGroupTitle(groupId));

			bool inserted = false;
			for (int i = 0; i < m_ui.listContacts->count(); ++i) {
				if (*glwi < *m_ui.listContacts->item(i)) {
					m_ui.listContacts->insertItem(i, glwi);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				m_ui.listContacts->addItem(glwi);
			}
		}
	}
}

void Client::messageCenterOnHasUnreadMessages(openmittsu::widgets::ChatTab* tab) {
	LOGGER_DEBUG("Activating window for unread messages...");
	if (m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_FORCE_FOREGROUND_ON_MESSAGE_RECEIVED)) {
		this->activateWindow();
	}

	if (m_optionMaster->getOptionAsBool(openmittsu::utility::OptionMaster::Options::BOOLEAN_PLAY_SOUND_ON_MESSAGE_RECEIVED)) {
		if (m_audioNotifier) {
			m_audioNotifier->playNotification();
		}
	}
}

void Client::protocolClientOnReadyConnect() {
	LOGGER_DEBUG("In Client: protocolClientOnReadyConnect()");
	m_connectionState = ConnectionState::STATE_DISCONNECTED;
	m_ui.btnConnect->setText("Connect");
	m_ui.btnConnect->setEnabled(true);
	uiFocusOnOverviewTab();
}

void Client::protocolClientOnLostConnection() {
	LOGGER_DEBUG("In Client: protocolClientOnLostConnection()");
	m_connectionState = ConnectionState::STATE_DISCONNECTED;
	m_ui.btnConnect->setText("Connect");
	m_ui.btnConnect->setEnabled(true);
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
		m_ui.btnConnect->setText(tr("Disconnect"));
		m_ui.btnConnect->setEnabled(true);

		// Start checking feature levels and statuses of contacts
		if (m_database) {
			QSet<openmittsu::protocol::ContactId> const contactsRequiringAccountStatusCheck = m_database->getContactsRequiringAccountStatusCheck(86400);
			QSet<openmittsu::protocol::ContactId> const contactsRequiringFeatureLevelCheck = m_database->getContactsRequiringFeatureLevelCheck(86400);
			
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
		m_ui.btnConnect->setText(tr("Connect"));
		m_ui.btnConnect->setEnabled(true);
	}
}

void Client::uiFocusOnOverviewTab() {
	m_ui.tabWidget->setCurrentWidget(m_ui.tabOverview);
}

void Client::listContactsOnDoubleClick(QListWidgetItem* item) {
	ContactListWidgetItem* clwi = dynamic_cast<ContactListWidgetItem*>(item);
	GroupListWidgetItem* glwi = dynamic_cast<GroupListWidgetItem*>(item);

	if ((m_database == nullptr) || (m_messageCenter == nullptr) || (m_tabController == nullptr)) {
		return;
	}

	if (clwi != nullptr) {
		openmittsu::protocol::ContactId const contactId = clwi->getContactId();
		m_tabController->openTab(contactId, m_database->getBackedContact(contactId, *m_messageCenter));
		m_tabController->focusTab(contactId);
	} else if (glwi != nullptr) {
		openmittsu::protocol::GroupId const groupId = glwi->getGroupId();
		m_tabController->openTab(groupId, m_database->getBackedGroup(groupId, *m_messageCenter));
		m_tabController->focusTab(groupId);
	} else {
		LOGGER()->warn("Could not determine the type of element the user double clicked on in the contacts list.");
		return;
	}
}	

void Client::listContactsOnContextMenu(QPoint const& pos) {
	QPoint globalPos = m_ui.listContacts->viewport()->mapToGlobal(pos);

	QListWidgetItem* listItem = m_ui.listContacts->itemAt(pos);

	ContactListWidgetItem* clwi = dynamic_cast<ContactListWidgetItem*>(listItem);
	GroupListWidgetItem* glwi = dynamic_cast<GroupListWidgetItem*>(listItem);

	if ((m_database == nullptr) || (m_messageCenter == nullptr) || (m_tabController == nullptr)) {
		return;
	}

	if ((clwi != nullptr) || (glwi != nullptr)) {
		QMenu listContactsContextMenu;

		QAction* actionHeadline = nullptr;
		QAction* actionEdit = nullptr;
		QAction* actionOpenClose = nullptr;
		QAction* actionRequestSync = nullptr;

		bool isChatWindowOpen = false;
		bool isIdentityContact = false;
		bool isGroupSelfOwned = false;
		//ChatTab* tab = nullptr;
		if (clwi != nullptr) {
			isIdentityContact = true;

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
			openmittsu::protocol::AccountStatus const status = m_database->getContactAccountStatus(clwi->getContactId());
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

			QSet<openmittsu::protocol::GroupId> const groups = m_database->getKnownGroupsContainingMember(clwi->getContactId());
			if (groups.size() < 1) {
				QAction* groupMembership = new QAction(tr("Not a member of any known group"), &listContactsContextMenu);
				groupMembership->setDisabled(true);
				listContactsContextMenu.addAction(groupMembership);
			} else {
				QAction* groupMembership = new QAction(tr("Group memberships:"), &listContactsContextMenu);
				groupMembership->setDisabled(true);
				listContactsContextMenu.addAction(groupMembership);

				for (openmittsu::protocol::GroupId const& groupId : groups) {
					QAction* groupMember = new QAction(QString(" - ").append(m_database->getGroupTitle(groupId)), &listContactsContextMenu);
					groupMember->setDisabled(true);
					listContactsContextMenu.addAction(groupMember);
				}
			}
		} else if (glwi != nullptr) {
			isIdentityContact = false;

			actionHeadline = new QAction(QString(tr("Group: %1")).arg(glwi->getGroupId().toQString()), &listContactsContextMenu);
			listContactsContextMenu.addAction(actionHeadline);
			actionEdit = new QAction("Edit Group", &listContactsContextMenu);
			listContactsContextMenu.addAction(actionEdit);

			isChatWindowOpen = m_tabController->hasTab(glwi->getGroupId());
			if (isChatWindowOpen) {
				actionOpenClose = new QAction(tr("Close Chat Window"), &listContactsContextMenu);
			} else {
				actionOpenClose = new QAction(tr("Open Chat Window"), &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionOpenClose);

			if (glwi->getGroupId().getOwner() == m_database->getSelfContact()) {
				isGroupSelfOwned = true;
				actionRequestSync = new QAction(tr("Force Group Sync"), &listContactsContextMenu);
			} else {
				actionRequestSync = new QAction(tr("Request Group Sync"), &listContactsContextMenu);
			}
			listContactsContextMenu.addAction(actionRequestSync);
			if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected()) {
				actionRequestSync->setDisabled(true);
			}

			QAction* separator = new QAction(&listContactsContextMenu);
			separator->setSeparator(true);
			listContactsContextMenu.addAction(separator);

			QAction* groupMembers = new QAction(tr("Group members:"), &listContactsContextMenu);
			groupMembers->setDisabled(true);
			listContactsContextMenu.addAction(groupMembers);

			QSet<openmittsu::protocol::ContactId> const members = m_database->getGroupMembers(glwi->getGroupId(), false);

			for (openmittsu::protocol::ContactId const& member : members) {
				QAction* groupMember = new QAction(QString(" - ").append(m_database->getContactNickname(member)), &listContactsContextMenu);
				groupMember->setDisabled(true);
				listContactsContextMenu.addAction(groupMember);
			}
		}

		QAction* selectedItem = listContactsContextMenu.exec(globalPos);
		if (selectedItem != nullptr) {
			if (selectedItem == actionEdit) {
				if (isIdentityContact) {
					QString const id = clwi->getContactId().toQString();
					QString const pubKey = m_database->getContactPublicKey(clwi->getContactId()).toQString();
					QString const nickname = m_database->getContactNickname(clwi->getContactId());

					ContactEditDialog contactEditDialog(id, pubKey, nickname, this);
					
					int result = contactEditDialog.exec();

					if (result == QDialog::DialogCode::Accepted) {
						QString const newNickname = contactEditDialog.getNickname();
						if (nickname != newNickname) {
							m_database->setContactNickname(clwi->getContactId(), newNickname);
						}
					}
				} else {
					showNotYetImplementedInfo();
				}
			} else if (selectedItem == actionOpenClose) {
				if (isChatWindowOpen) {
					if (isIdentityContact) {
						m_tabController->closeTab(clwi->getContactId());
					} else {
						m_tabController->closeTab(glwi->getGroupId());
					}
				} else {
					if (isIdentityContact) {
						m_tabController->openTab(clwi->getContactId(), m_database->getBackedContact(clwi->getContactId(), *m_messageCenter));
						m_tabController->focusTab(clwi->getContactId());
					} else {
						m_tabController->openTab(glwi->getGroupId(), m_database->getBackedGroup(glwi->getGroupId(), *m_messageCenter));
						m_tabController->focusTab(glwi->getGroupId());
					}
				}
			} else if (!isIdentityContact && (selectedItem == actionRequestSync) && (actionRequestSync != nullptr)) {
				if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected() || (m_messageCenter == nullptr)) {
					return;
				}

				if (isGroupSelfOwned) {
					m_messageCenter->resendGroupSetup(glwi->getGroupId());
				} else {
					m_messageCenter->sendSyncRequest(glwi->getGroupId());
				}
			}
		}
	}
}

/*
	MENU ENTRIES & HANDLING
*/

void Client::menuFileOptionsOnClick() {
	if (m_database == nullptr || m_optionMaster == nullptr) {
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
	LicenseDialog* licenseDialog = new LicenseDialog(this);
	licenseDialog->exec();
}

void Client::menuAboutAboutOnClick() {
	QMessageBox::about(this, "OpenMittsu - About", QString("<h2>OpenMittsu</h2><br><br>%1<br>%2<br><br>An open source chat client for Threema-style end-to-end encrypted chat networks.<br><br>This project is in no way connected, affiliated or endorsed with/by Threema GmbH.<br><br>Copyright (C) 2015-17 by Philipp Berger<br>This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.<br>See LICENSE for further information.<br><br>Don't be a jerk!").arg(QString::fromStdString(openmittsu::utility::Version::longVersionString())).arg(QString::fromStdString(openmittsu::utility::Version::buildInfo())));
}

void Client::menuAboutAboutQtOnClick() {
	QMessageBox::aboutQt(this, "About Qt");
}

void Client::menuGroupAddOnClick() {
	if ((m_database == nullptr) || (m_messageCenter == nullptr)) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else if (m_protocolClient == nullptr || !m_protocolClient->getIsConnected()) {
		QMessageBox::warning(this, "Not connected to a server", "Before you can use this feature you need to connect to a server.");
	} else {
		openmittsu::wizards::GroupCreationWizard groupCreationWizard(m_database->getKnownContactsWithNicknames(false), std::make_unique<openmittsu::dataproviders::SimpleGroupCreationProcessor>(m_messageCenter), this);
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

void Client::menuGroupEditOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuGroupLeaveOnClick() {
	showNotYetImplementedInfo();
}

void Client::menuContactAddOnClick() {
	if (m_database == nullptr) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
		return;
	}

	ContactAddDialog contactAddDialog(this);
	int result = contactAddDialog.exec();

	if (result == QDialog::DialogCode::Accepted) {
		QString const identityString = contactAddDialog.getIdentity();
		QString const nickname = contactAddDialog.getNickname();
		if (identityString.isEmpty() || (identityString.size() != 8)) {
			QMessageBox::warning(this, "Could not add Contact", "The identity entered is not well formed.\nNo contact has been added.");
		} else if (m_database->hasContact(openmittsu::protocol::ContactId(identityString.toUtf8()))) {
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
					m_database->storeNewContact(contactId, ir.getFetchedPublicKey());
					if (!nickname.isEmpty()) {
						m_database->setContactNickname(contactId, nickname);
					}

					QMessageBox::information(this, "Contact added", QString("Contact successfully added!\nIdentity: %1\nPublic Key: %2\n").arg(contactId.toQString()).arg(ir.getFetchedPublicKey().toQString()));
				}
			} catch (openmittsu::exceptions::ProtocolErrorException& pee) {
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
	if (m_database == nullptr) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		openmittsu::backup::IdentityBackup const backupData = m_database->getBackup();
		openmittsu::dialogs::FingerprintDialog fingerprintDialog(backupData.getClientContactId(), backupData.getClientLongTermKeyPair(), this);
		fingerprintDialog.exec();
	}
}

void Client::menuIdentityShowPublicKeyOnClick() {
	if (m_database == nullptr) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		openmittsu::backup::IdentityBackup const backupData = m_database->getBackup();
		openmittsu::dialogs::ShowIdentityAndPublicKeyDialog showIdentityAndPublicKeyDialog(backupData.getClientContactId(), backupData.getClientLongTermKeyPair(), this);
		showIdentityAndPublicKeyDialog.exec();
	}
}

void Client::menuIdentityCreateBackupOnClick() {
	if (m_database == nullptr) {
		QMessageBox::warning(this, "No database loaded", "Before you can use this feature you need to load a database from file (see main screen) or create one using a backup of your existing ID (see Identity -> Load Backup).");
	} else {
		openmittsu::wizards::BackupCreationWizard backupCreationWizard(m_database.get()->getBackup(), this);
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
	if (m_database == nullptr) {
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
				QHash<openmittsu::protocol::ContactId, std::pair<openmittsu::crypto::PublicKey, QString>> const& contacts = lci.getContacts();
				auto it = contacts.constBegin();
				auto end = contacts.constEnd();
				for (; it != end; ++it) {
					m_database->storeNewContact(it.key(), it.value().first, openmittsu::protocol::ContactIdVerificationStatus::VERIFICATION_STATUS_SERVER_VERIFIED, "", "", it.value().second, 0);
				}
			}

			{
				QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> const& groups = lci.getGroups();
				auto it = groups.constBegin();
				auto end = groups.constEnd();
				for (; it != end; ++it) {
					m_database->storeNewGroup(it.key(), it.value().second, openmittsu::protocol::MessageTime::now(), it.value().first, false, false);
				}
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

		m_ui.lblStatus->setText(text.arg(formatDuration(seconds)));
	} else {
		QString const text(QStringLiteral("Not connected"));
		m_ui.lblStatus->setText(text);
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
			openmittsu::protocol::ContactId const selfIdentity = (m_database == nullptr) ? openmittsu::protocol::ContactId(0) : m_database->getSelfContact();

			if (m_database) {
				m_database->setContactFeatureLevelBatch(checkFeatureLevelTask->getFetchedFeatureLevels());
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
					if ((m_database == nullptr) || (m_serverConfiguration == nullptr)) {
						LOGGER()->error("Wanted to update feature level, but either database or server config is null!");
						return;
					}

					openmittsu::backup::IdentityBackup const backupData = m_database->getBackup();
					openmittsu::crypto::BasicCryptoBox basicCryptoBox(backupData.getClientLongTermKeyPair(), m_serverConfiguration->getServerLongTermPublicKey());
					openmittsu::tasks::SetFeatureLevelCallbackTask* setFeatureLevelTask = new openmittsu::tasks::SetFeatureLevelCallbackTask(m_serverConfiguration, basicCryptoBox, backupData.getClientContactId(), openMittsuFeatureLevel);
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
			if (m_database == nullptr) {
				LOGGER()->error("Wanted to update feature levels, but the database is null!");
				return;
			}

			m_database->setContactAccountStatusBatch(checkContactIdStatusTask->getFetchedStatus());
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
