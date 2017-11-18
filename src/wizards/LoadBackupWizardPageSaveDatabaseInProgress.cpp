#include "src/wizards/LoadBackupWizardPageSaveDatabaseInProgress.h"
#include "ui_LoadBackupWizardPageSaveDatabaseInProgress.h"

#include "src/backup/BackupReader.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/wizards/LoadBackupWizard.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageSaveDatabaseInProgress::LoadBackupWizardPageSaveDatabaseInProgress(LoadBackupWizard* parent) : QWizardPage(parent), m_parent(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageSaveDatabaseInProgress>()), m_isComplete(false), m_completedWithSuccess(false) {
			m_ui->setupUi(this);
		}

		LoadBackupWizardPageSaveDatabaseInProgress::~LoadBackupWizardPageSaveDatabaseInProgress() {
			//
		}

		int LoadBackupWizardPageSaveDatabaseInProgress::nextId() const {
			return LoadBackupWizard::Pages::PAGE_DONE;
		}

		void LoadBackupWizardPageSaveDatabaseInProgress::initializePage() {
			QString const databaseLocationString = field("edtSaveDatabaseLocation").toString();
			QString const databasePassword = field("edtSaveDatabasePassword").toString();
			QDir databaseLocation(databaseLocationString);

			QString const backupLocationString = field("edtDataBackupLocation").toString();
			QString const backupPassword = field("edtDataBackupPassword").toString();
			QDir backupLocation(backupLocationString);

			if (m_backupReader != nullptr) {
				OPENMITTSU_DISCONNECT_NOTHROW(m_backupReader.get(), progressUpdated(int), this, onProgressUpdated(int));
				OPENMITTSU_DISCONNECT_NOTHROW(m_backupReader.get(), finished(bool, QString const&), this, onFinished(bool, QString const&));
			}

			m_databaseFileName = databaseLocation.absoluteFilePath(openmittsu::database::Database::getDefaultDatabaseFileName());
			m_backupReader = std::make_unique<openmittsu::backup::BackupReader>(backupLocation, backupPassword, m_databaseFileName, databaseLocation, databasePassword);
			OPENMITTSU_CONNECT_QUEUED(m_backupReader.get(), progressUpdated(int), this, onProgressUpdated(int));
			OPENMITTSU_CONNECT_QUEUED(m_backupReader.get(), finished(bool, QString const&), this, onFinished(bool, QString const&));

			m_backupReader->start();
		}

		void LoadBackupWizardPageSaveDatabaseInProgress::cleanupPage() {
			if (m_backupReader != nullptr) {
				OPENMITTSU_DISCONNECT_NOTHROW(m_backupReader.get(), progressUpdated(int), this, onProgressUpdated(int));
				OPENMITTSU_DISCONNECT_NOTHROW(m_backupReader.get(), finished(bool, QString const&), this, onFinished(bool, QString const&));
				m_backupReader->quit();
				m_backupReader->wait(30000);
				m_backupReader.reset();
			}
		}

		bool LoadBackupWizardPageSaveDatabaseInProgress::isComplete() const {
			return m_isComplete && m_completedWithSuccess;
		}

		bool LoadBackupWizardPageSaveDatabaseInProgress::completedWithSuccess() const {
			return m_completedWithSuccess;
		}

		void LoadBackupWizardPageSaveDatabaseInProgress::onProgressUpdated(int percentComplete) {
			m_ui->progressBar->setValue(percentComplete);
		}

		void LoadBackupWizardPageSaveDatabaseInProgress::onFinished(bool hadError, QString const& errorMessage) {
			m_isComplete = true;

			if (hadError) {
				m_ui->lblStatus->setText(QStringLiteral("Import failed!\n").append(errorMessage));
			} else {
				m_ui->lblStatus->setText(QStringLiteral("Import successful!\n"));
				m_completedWithSuccess = true;
				if (m_parent != nullptr) {
					m_parent->setDatabaseFileName(m_databaseFileName);
				}
			}

			emit completeChanged();
		}

	}
}
