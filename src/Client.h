#ifndef OPENMITTSU_CLIENT_H
#define OPENMITTSU_CLIENT_H

#include <QString>
#include <QSettings>
#include <QMainWindow>
#include <QThread>
#include <QHash>
#include <QTimer>
#include <QFile>
#include <QAudioOutput>

#include <memory>

#include "ui_main.h"
#include "ProtocolClient.h"
#include "ContactRegistry.h"
#include "ChatTab.h"
#include "updater/Updater.h"

#include "protocol/KeyRegistry.h"
#include "protocol/GroupRegistry.h"

class Client : public QMainWindow {
    Q_OBJECT

public:
    Client(QWidget *parent = 0);
	~Client();
private slots:
	// UI
	void btnConnectOnClick();
	void btnOpenClientIniOnClick();
	void btnOpenContactsOnClick();
	void listContactsOnDoubleClick(QListWidgetItem* item);
	void listContactsOnContextMenu(QPoint const& pos);
	void chatTabWidgetOnCurrentTabChanged(int index);

	void menuFileOptionsOnClick();
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

	// Audio
	void audioOutputOnStateChanged(QAudio::State state);

	// Updater
	void updaterFoundNewVersion(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString gitHash, QString channel, QString link);
public slots:
	void contactRegistryOnIdentitiesChanged();
	void messageCenterOnHasUnreadMessages(ChatTab*);
	void connectionTimerOnTimer();

	// All messages from the ProtocolClient
	void protocolClientOnReadyConnect();
	void protocolClientOnLostConnection();
	void protocolClientOnConnectToFinished(int errCode, QString message);
	void protocolClientOnDuplicateIdUsageDetected();

	// Tasks
	void callbackTaskFinished(CallbackTask* callbackTask);

	// Thread Handling
	void threadFinished();
protected:
	virtual void closeEvent(QCloseEvent* event) override;
private:
	Ui::MainWindow ui;
	std::unique_ptr<ProtocolClient> protocolClient;
	QThread protocolClientThread;
	QTimer connectionTimer;

	// Audio playing
	QFile receivedMessageAudioFile;
	std::unique_ptr<QAudioOutput> audioOutput;

	// Update functionality
	Updater updater;

	// State of Connection
	enum class ConnectionState {
		STATE_DISCONNECTED,
		STATE_CONNECTING,
		STATE_CONNECTED
	};

	ConnectionState connectionState;
	std::unique_ptr<ServerConfiguration> serverConfiguration;
	std::unique_ptr<ClientConfiguration> clientConfiguration;
	std::unique_ptr<ContactRegistry> contactRegistry;

	bool validateClientConfigurationFile(QString const& fileName, bool quiet = false);
	bool validateKnownIdentitiesFile(QString const& fileName, bool quiet = false);
	void updateClientSettingsInfo(QString const& currentFileName);
	void updateKnownContactsInfo(QString const& currentFileName);
	void uiFocusOnOverviewTab();
	void showNotYetImplementedInfo();
	void setupProtocolClient();

	QString formatDuration(quint64 duration) const;
	void importLegacyOptions();
};

#endif