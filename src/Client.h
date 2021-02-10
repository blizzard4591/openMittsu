#ifndef OPENMITTSU_CLIENT_H
#define OPENMITTSU_CLIENT_H

#include <QAudioOutput>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>

#include <memory>

#include "src/database/Database.h"
#include "src/database/DatabasePointerAuthority.h"
#include "src/database/DatabaseWrapper.h"

#include "src/dataproviders/MessageCenterPointerAuthority.h"
#include "src/dataproviders/MessageCenterWrapper.h"

#include "src/dataproviders/KeyRegistry.h"

#include "src/network/ProtocolClient.h"

#include "src/options/OptionMaster.h"

#include "src/tasks/CallbackTask.h"

#include "src/updater/Updater.h"

#include "src/utility/AudioNotification.h"
#include "src/utility/ThreadContainer.h"

#include "src/widgets/DesktopNotification.h"
#include "src/widgets/TabController.h"

namespace Ui {
	class MainWindow;
}

class Client : public QMainWindow {
	Q_OBJECT
public:
	Client(QWidget* parent = nullptr);
	virtual ~Client();
private slots:
	// UI
	void btnConnectOnClick();
	void btnOpenDatabaseOnClick();
	void listContactsOnDoubleClick(QListWidgetItem* item);
	void listContactsOnContextMenu(QPoint const& pos);
	void listGroupsOnDoubleClick(QListWidgetItem* item);
	void listGroupsOnContextMenu(QPoint const& pos);

	void menuFileOptionsOnClick();
	void menuFileShowFirstUseWizardOnClick();
	void menuFileExitOnClick();
	void menuAboutLicenseOnClick();
	void menuAboutAboutOnClick();
	void menuAboutAboutQtOnClick();
	void menuAboutStatisticsOnClick();
	void menuViewShowEmojiTabOnTriggered(bool checked);
	void menuGroupAddOnClick();
	void menuGroupEditOnClick();
	void menuGroupLeaveOnClick();
	void menuContactAddOnClick();
	void menuContactEditOnClick();
	void menuContactDeleteOnClick();
	void menuContactSaveToFileOnClick();
	void menuIdentityShowFingerprintOnClick();
	void menuIdentityShowPublicKeyOnClick();
	void menuIdentityCreateBackupOnClick();
	void menuIdentityLoadBackupOnClick(QString const& legacyClientConfigurationFileName = "");
	void menuDatabaseImportLegacyContactsAndGroupsOnClick(QString const& legacyContactsFileName = "");

	// Updater
	void updaterFoundNewVersion(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString gitHash, QString channel, QString link);

	void delayedStartup();
public slots:
	void contactRegistryOnIdentitiesChanged();
	void connectionTimerOnTimer();
	void unreadMessagesIconBlinkTimerOnTimer();

	void onDatabaseUpdated();
	void onDatabaseContactChanged(openmittsu::protocol::ContactId const& contact);
	void onDatabaseGroupChanged(openmittsu::protocol::GroupId const& group);
	void onDatabaseReceivedNewContactMessage(openmittsu::protocol::ContactId const& contact);
	void onDatabaseReceivedNewGroupMessage(openmittsu::protocol::GroupId const& group);
	
	void onMessageCenterHasUnreadMessageContact(openmittsu::protocol::ContactId const& contact);
	void onMessageCenterHasUnreadMessageGroup(openmittsu::protocol::GroupId const& group);

	// All messages from the ProtocolClient
	void protocolClientOnReadyConnect();
	void protocolClientOnLostConnection();
	void protocolClientOnConnectToFinished(int errCode, QString message);
	void protocolClientOnDuplicateIdUsageDetected();

	// Tasks
	void callbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask);

	// Thread Handling
	void threadFinished();
protected:
	virtual void closeEvent(QCloseEvent* event) override;
private:
	bool m_isClosing;
	std::unique_ptr<Ui::MainWindow> m_ui;
	std::shared_ptr<openmittsu::network::ProtocolClient> m_protocolClient;
	QThread m_protocolClientThread;
	QTimer m_connectionTimer;
	QTimer m_unreadMessagesIconBlinkTimer;
	bool m_unreadMessagesIconBlinkState;

	// Update functionality
	openmittsu::updater::Updater m_updater;

	// State of Connection
	enum class ConnectionState {
		STATE_DISCONNECTED,
		STATE_CONNECTING,
		STATE_CONNECTED
	};

	ConnectionState m_connectionState;
	std::shared_ptr<openmittsu::widgets::TabController> m_tabController;
	
	openmittsu::utility::MessageCenterThreadContainer m_messageCenterThread;
	openmittsu::dataproviders::MessageCenterPointerAuthority m_messageCenterPointerAuthority;
	openmittsu::dataproviders::MessageCenterWrapper m_messageCenterWrapper;

	std::shared_ptr<openmittsu::network::ServerConfiguration> m_serverConfiguration;
	std::shared_ptr<openmittsu::options::OptionMaster> m_optionMaster;

	openmittsu::utility::DatabaseThreadContainer m_databaseThread;
	openmittsu::database::DatabasePointerAuthority m_databasePointerAuthority;
	openmittsu::database::DatabaseWrapper m_databaseWrapper;

	std::shared_ptr<openmittsu::utility::AudioNotification> m_audioNotifier;
	std::shared_ptr<openmittsu::widgets::DesktopNotification> m_desktopNotifier;

	// Commandline options
	bool m_optionTryEmptyPassword;
	bool m_optionAutoConnect;
	bool m_optionMinimize;
	bool m_optionUsePasswordFile;
	QString m_optionPasswordFromFile;
	bool m_optionUseDatabaseFile;
	QString m_optionDatabaseFile;

	void openDatabaseFile(QString const& fileName);
	bool validateDatabaseFile(QString const& databaseFileName, QString const& password, bool quiet = false);
	void updateDatabaseInfo(QString const& currentFileName);
	void askForDatabaseRemovalFromConfig(QString const& databaseLocation);
	void uiFocusOnOverviewTab();
	void showNotYetImplementedInfo();
	void setupProtocolClient();

	QString formatDuration(quint64 duration) const;
	void onHasUnreadMessage(openmittsu::widgets::ChatTab* tab);
	void setAppIcon(bool haveUnreadMessages);
};

#endif
