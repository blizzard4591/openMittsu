#include "src/wizards/LoadBackupWizard.h"
#include "ui_LoadBackupWizard.h"

#include <QMessageBox>
#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InternalErrorException.h"

#include "src/backup/IdentityBackup.h"
#include "src/backup/BackupReader.h"
#include "src/database/Database.h"
#include "src/utility/LegacyClientConfigurationImporter.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizard::LoadBackupWizard(QString const& legacyClientConfigurationFileName, QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::LoadBackupWizard>()), m_hasPresetBackup(false) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			m_loadBackupWizardPageSelectBackupType = std::make_unique<LoadBackupWizardPageSelectBackupType>(this);
			this->setPage(Pages::PAGE_SELECTBACKUPTYPE, m_loadBackupWizardPageSelectBackupType.get());

			m_loadBackupWizardPageDataBackupData = std::make_unique<LoadBackupWizardPageDataBackupData>(this);
			this->setPage(Pages::PAGE_DATABACKUP_DATA, m_loadBackupWizardPageDataBackupData.get());

			m_loadBackupWizardPageIdBackupData = std::make_unique<LoadBackupWizardPageIdBackupData>(this);
			this->setPage(Pages::PAGE_IDBACKUP_DATA, m_loadBackupWizardPageIdBackupData.get());

			m_loadBackupWizardPageLegacyClientConfigurationData = std::make_unique<LoadBackupWizardPageLegacyClientConfigurationData>(legacyClientConfigurationFileName, this);
			this->setPage(Pages::PAGE_LEGACYCLIENTCONFIGURATION, m_loadBackupWizardPageLegacyClientConfigurationData.get());

			m_loadBackupWizardPageSaveDatabase = std::make_unique<LoadBackupWizardPageSaveDatabase>(*this, this);
			this->setPage(Pages::PAGE_SELECTTARGET, m_loadBackupWizardPageSaveDatabase.get());

			m_loadBackupWizardPageSaveDatabaseInProgress = std::make_unique<LoadBackupWizardPageSaveDatabaseInProgress>(this);
			this->setPage(Pages::PAGE_DATABASE_SAVE_IN_PROGRESS, m_loadBackupWizardPageSaveDatabaseInProgress.get());

			m_loadBackupWizardPageDone = std::make_unique<LoadBackupWizardPageDone>(this);
			this->setPage(Pages::PAGE_DONE, m_loadBackupWizardPageDone.get());

			setOption(QWizard::NoBackButtonOnLastPage, true);
			
			if (legacyClientConfigurationFileName.isEmpty()) {
				this->setStartId(Pages::PAGE_SELECTBACKUPTYPE);
			} else {
				if (LoadBackupWizardPageLegacyClientConfigurationData::isValidClientConfiguration(legacyClientConfigurationFileName, true, this)) {
					m_hasPresetBackup = true;
					this->setStartId(Pages::PAGE_SELECTTARGET);
				} else {
					this->setStartId(Pages::PAGE_LEGACYCLIENTCONFIGURATION);
				}
			}
		}

		LoadBackupWizard::~LoadBackupWizard() {
			//
		}

		void LoadBackupWizard::pageNextOnClick(int pageId) {
			if (page(pageId) == m_loadBackupWizardPageSaveDatabase.get()) {
				if (hasVisitedPage(Pages::PAGE_IDBACKUP_DATA)) {
					QString const backupString = field("edtIdBackupBackupString").toString().toUpper();
					QString const passwordString = field("edtIdBackupPassword").toString();

					try {
						openmittsu::backup::IdentityBackup data(openmittsu::backup::IdentityBackup::fromBackupString(backupString, passwordString));

						m_backupString = backupString;
						m_backupPassword = passwordString;
					} catch (openmittsu::exceptions::InvalidInputException& iie) {
						back();
						QMessageBox::warning(this, tr("Incorrect Password or Backup"), tr("Either the password is incorrect or you mistyped the Backup string!\nProblem: %1").arg(iie.what()));
					}
				} else if (hasVisitedPage(Pages::PAGE_DATABACKUP_DATA)) {
					QString const backupLocationString = field("edtDataBackupLocation").toString();
					QString const passwordString = field("edtDataBackupPassword").toString();

					QDir location(backupLocationString);
					if (!location.exists()) {
						back();
						QMessageBox::warning(this, tr("Incorrect Backup Location"), tr("The chosen backup location '%1' does not exist or is not readable!").arg(location.absolutePath()));
						return;
					}
					
					QFile file(location.absoluteFilePath(QStringLiteral("identity")));
					if (!file.exists() || !file.open(QFile::ReadOnly)) {
						back();
						QMessageBox::warning(this, tr("Incorrect Backup Location"), tr("The chosen backup location '%1' does not contain the 'identity' file or is not readable!").arg(location.absolutePath()));
						return;
					}

					QString const backupString = QString::fromUtf8(file.readAll());
					file.close();

					try {
						openmittsu::backup::IdentityBackup data(openmittsu::backup::IdentityBackup::fromBackupString(backupString, passwordString));

						m_backupString = backupString;
						m_backupPassword = passwordString;
					} catch (openmittsu::exceptions::InvalidInputException& iie) {
						back();
						QMessageBox::warning(this, tr("Incorrect Password or Backup"), tr("Either the password is incorrect or your backup is corrupted!\nProblem: %1").arg(iie.what()));
					}
				} else {
					QString const clientConfigurationFilePath = field("edtClientConfigurationLocation").toString();

					if (LoadBackupWizardPageLegacyClientConfigurationData::isValidClientConfiguration(clientConfigurationFilePath, false)) {
						try {
							openmittsu::utility::LegacyClientConfigurationImporter lcci(openmittsu::utility::LegacyClientConfigurationImporter::fromFile(clientConfigurationFilePath));

							m_backupString = lcci.getBackupString();
							m_backupPassword = lcci.getBackupPassword();
						} catch (...) {
							back();
							QMessageBox::warning(this, tr("Incorrect client configuration"), tr("Something went wrong!\nDid you delete the file?"));
						}
					} else {
						back();
						QMessageBox::warning(this, tr("Incorrect client configuration"), tr("Something went wrong!\nDid you delete the file?"));
					}
				}
			} else if (page(pageId) == m_loadBackupWizardPageDone.get()) {
				QString const databaseLocationString = field("edtSaveDatabaseLocation").toString();
				QString const databasePassword = field("edtSaveDatabasePassword").toString();
				QDir databaseLocation(databaseLocationString);

				if (hasVisitedPage(Pages::PAGE_IDBACKUP_DATA) || hasVisitedPage(Pages::PAGE_LEGACYCLIENTCONFIGURATION) || m_hasPresetBackup) {
					try {
						openmittsu::backup::IdentityBackup data(openmittsu::backup::IdentityBackup::fromBackupString(m_backupString, m_backupPassword));

						m_databaseFileName = "";
						QString const dbFileName = databaseLocation.absoluteFilePath(openmittsu::database::Database::getDefaultDatabaseFileName());
						openmittsu::database::Database db(dbFileName, data.getClientContactId(), data.getClientLongTermKeyPair(), databasePassword, databaseLocation);
						m_databaseFileName = dbFileName;
					} catch (openmittsu::exceptions::BaseException& be) {
						back();
						QMessageBox::warning(this, tr("Error while writing Database"), tr("An error occured while writing the database.\nProblem: %1").arg(be.what()));
					} catch (...) {
						back();
						QMessageBox::warning(this, tr("Error while writing Database"), tr("An unexpected error occured while writing the database. Is the selected folder writable?"));
					}
				}
			}
		}

		QString LoadBackupWizard::getDatabaseFileName() const {
			return m_databaseFileName;
		}

		void LoadBackupWizard::setDatabaseFileName(QString const& fileName) {
			m_databaseFileName = fileName;
		}

	}
}
