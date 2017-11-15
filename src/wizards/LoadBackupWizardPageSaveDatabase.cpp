#include "src/wizards/LoadBackupWizardPageSaveDatabase.h"
#include "ui_LoadBackupWizardPageSaveDatabase.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/wizards/LoadBackupWizard.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageSaveDatabase::LoadBackupWizardPageSaveDatabase(LoadBackupWizard const& loadBackupWizard, QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageSaveDatabase>()), m_loadBackupWizard(loadBackupWizard) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(m_ui->btnPickSave, clicked(), this, btnPickSaveOnClick());

			m_fileNameValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression(".+"));
			m_ui->edtSaveDatabaseLocation->setValidator(m_fileNameValidator.get());

			registerField("edtSaveDatabaseLocation*", m_ui->edtSaveDatabaseLocation);
			registerField("edtSaveDatabasePassword*", m_ui->edtSaveDatabasePassword);
		}

		LoadBackupWizardPageSaveDatabase::~LoadBackupWizardPageSaveDatabase() {
			//
		}

		int LoadBackupWizardPageSaveDatabase::nextId() const {
			if (m_loadBackupWizard.hasVisitedPage(LoadBackupWizard::Pages::PAGE_IDBACKUP_DATA)) {
				return LoadBackupWizard::Pages::PAGE_DONE;
			} else {
				return LoadBackupWizard::Pages::PAGE_DATABASE_SAVE_IN_PROGRESS;
			}
		}

		void LoadBackupWizardPageSaveDatabase::initializePage() {
			m_ui->edtSaveDatabaseLocation->setText("");
		}

		bool LoadBackupWizardPageSaveDatabase::isComplete() const {
			return !m_ui->edtSaveDatabaseLocation->text().isEmpty();
		}

		void LoadBackupWizardPageSaveDatabase::btnPickSaveOnClick() {
			QString const folderName = QFileDialog::getExistingDirectory(this, tr("Database storage location"));
			if (folderName.isEmpty() || folderName.isNull()) {
				m_ui->edtSaveDatabaseLocation->setText("");
			} else {
				QDir folder(folderName);
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
				if (!folder.exists() || !folder.isEmpty()) {
#else
				if (!folder.exists() || folder.entryList(QDir::NoDotAndDotDot).size() > 0) {
#endif
					m_ui->edtSaveDatabaseLocation->setText("");
					QMessageBox::warning(this, tr("Invalid database storage location"), tr("Please select an existing and empty folder!"));
				} else {
					m_ui->edtSaveDatabaseLocation->setText(folderName);

					emit completeChanged();
				}
			}
		}

	}
}
