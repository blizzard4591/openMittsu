#include "src/wizards/BackupCreationWizardPageBackup.h"
#include "ui_backupcreationwizardpagebackup.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace wizards {
		BackupCreationWizardPageBackup::BackupCreationWizardPageBackup(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::BackupCreationWizardPageBackup>()), m_identityBackup(identityBackup) {
			m_ui->setupUi(this);

			registerField("edtBackupString*", m_ui->edtBackupString);
		}

		BackupCreationWizardPageBackup::~BackupCreationWizardPageBackup() {
			//
		}

		void BackupCreationWizardPageBackup::initializePage() {
			QString const password = field("edtPassword").toString();
			QString const backup = m_identityBackup.toBackupString(password);
			m_ui->edtBackupString->setText(backup.toUpper());
			m_ui->edtBackupString->selectAll();
		}
	}
}
