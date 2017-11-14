#include "src/wizards/LoadBackupWizardPageDataBackupData.h"
#include "ui_LoadBackupWizardPageDataBackupData.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QString>

#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/wizards/LoadBackupWizard.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageDataBackupData::LoadBackupWizardPageDataBackupData(QWidget *parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageDataBackupData>()) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(m_ui->btnOpenFolder, clicked(), this, onBtnOpenBackupFolderClick());

			// At least one character
			m_passwordValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression(".+"));
			m_ui->edtDataBackupPassword->setValidator(m_passwordValidator.get());
			m_ui->edtDataBackupPassword->setEchoMode(QLineEdit::EchoMode::Password);
			m_backupPathValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression(".+"));
			m_ui->edtDataBackupLocation->setValidator(m_backupPathValidator.get());

			registerField("edtDataBackupPassword*", m_ui->edtDataBackupPassword);
			registerField("edtDataBackupLocation*", m_ui->edtDataBackupLocation);
		}

		LoadBackupWizardPageDataBackupData::~LoadBackupWizardPageDataBackupData() {
			//
		}

		int LoadBackupWizardPageDataBackupData::nextId() const {
			return LoadBackupWizard::Pages::PAGE_SELECTTARGET;
		}

		bool LoadBackupWizardPageDataBackupData::isValidBackupLocation(QDir const& location, bool showWarnings, QWidget* parent) {
			if (!location.exists()) {
				return false;
			}

			if (!QFile::exists(location.absoluteFilePath(QStringLiteral("identity")))) {
				if (showWarnings) {
					QMessageBox::warning(parent, tr("Invalid data backup location"), tr("The selected folder does not contain the 'identity' file, which is a required part of the data backup!"));
				}
				return false;
			} else if (!QFile::exists(location.absoluteFilePath(QStringLiteral("contacts.csv")))) {
				if (showWarnings) {
					QMessageBox::warning(parent, tr("Invalid data backup location"), tr("The selected folder does not contain the 'contacts.csv' file, which is a required part of the data backup!"));
				}
				return false;
			} else if (!QFile::exists(location.absoluteFilePath(QStringLiteral("groups.csv")))) {
				if (showWarnings) {
					QMessageBox::warning(parent, tr("Invalid data backup location"), tr("The selected folder does not contain the 'groups.csv' file, which is a required part of the data backup!"));
				}
				return false;
			}
			return true;
		}

		void LoadBackupWizardPageDataBackupData::onBtnOpenBackupFolderClick() {
			QString const folderName = QFileDialog::getExistingDirectory(this, tr("Select a folder containing a Threema data backup"));
			QDir const folder(folderName);
			if (folderName.isNull() || folderName.isEmpty()) {
				return;
			}

			if (!isValidBackupLocation(folder, true, this)) {
				m_ui->edtDataBackupLocation->setText("");
			} else {
				m_ui->edtDataBackupLocation->setText(folder.absolutePath());
			}
		}

	}
}
