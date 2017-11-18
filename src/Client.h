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
	void menuIdentityLoadBackupOnClick();
	void menuDatabaseImportLegacyContactsAndGroupsOnClick();

	// Updater
	void updaterFoundNewVersion(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString gitHash, QString channel, QString link);
public slots:
	void contactRegistryOnIdentitiesChanged();
	void messageCenterOnHasUnreadMessages(openmittsu::widgets::ChatTab*);
	void connectionTimerOnTimer();

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
	std::shared_ptr<openmittsu::dataproviders::MessageCenter> m_messageCenter;
	std::shared_ptr<openmittsu::network::ServerConfiguration> m_serverConfiguration;
	std::shared_ptr<openmittsu::utility::OptionMaster> m_optionMaster;
	std::shared_ptr<openmittsu::database::Database> m_database;
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
