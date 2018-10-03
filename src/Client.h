#ifndef OPENMITTSU_CLIENT_H
#define OPENMITTSU_CLIENT_H

#include <QAudioOutput>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>

#include <memory>

#include "ui_main.h"
#include "network/ProtocolClient.h"

#include "src/updater/Updater.h"

#include "src/database/Database.h"
#include "src/database/DatabasePointerAuthority.h"
#include "src/database/DatabaseWrapper.h"
#include "src/utility/ThreadContainer.h"

#include "src/dataproviders/KeyRegistry.h"
#include "src/widgets/TabController.h"

#include "src/utility/AudioNotification.h"
#include "src/tasks/CallbackTask.h"

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

	void menuFileOptionsOnClick();
	void menuFileShowFirstUseWizardOnClick();
	void menuFileExitOnClick();
	void menuAboutLicenseOnClick();
	void menuAboutAboutOnClick();
	void menuAboutAboutQtOnClick();
	void menuAboutStatisticsOnClick();
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
public slots:
	void contactRegistryOnIdentitiesChanged();
	void connectionTimerOnTimer();

	void databaseOnReceivedNewContactMessage(openmittsu::protocol::ContactId const& identity);
	void databaseOnReceivedNewGroupMessage(openmittsu::protocol::GroupId const& group);
	void onHasUnreadMessage(openmittsu::widgets::ChatTab* tab);

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
	Ui::MainWindow m_ui;
	std::shared_ptr<openmittsu::network::ProtocolClient> m_protocolClient;
	QThread m_protocolClientThread;
	QTimer m_connectionTimer;

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
	std::unique_ptr<openmittsu::network::MessageCenterWrapper> m_messageCenterWrapper;

	std::shared_ptr<openmittsu::network::ServerConfiguration> m_serverConfiguration;
	std::shared_ptr<openmittsu::utility::OptionMaster> m_optionMaster;

	openmittsu::utility::DatabaseThreadContainer m_databaseThread;
	openmittsu::database::DatabasePointerAuthority m_databasePointerAuthority;
	openmittsu::database::DatabaseWrapper m_databaseWrapper;

	std::shared_ptr<openmittsu::utility::AudioNotification> m_audioNotifier;

	void openDatabaseFile(QString const& fileName);
	bool validateDatabaseFile(QString const& databaseFileName, QString const& password, bool quiet = false);
	void updateDatabaseInfo(QString const& currentFileName);
	void uiFocusOnOverviewTab();
	void showNotYetImplementedInfo();
	void setupProtocolClient();

	QString formatDuration(quint64 duration) const;
};

#endif
