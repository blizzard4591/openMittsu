#include "src/wizards/BackupCreationWizardPageBackup.h"
#include "ui_backupcreationwizardpagebackup.h"

namespace openmittsu {
	namespace wizards {
		BackupCreationWizardPageBackup::BackupCreationWizardPageBackup(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent) : QWizardPage(parent), ui(new Ui::BackupCreationWizardPageBackup), m_identityBackup(identityBackup) {
			ui->setupUi(this);

			registerField("edtBackupString*", ui->edtBackupString);
		}

		BackupCreationWizardPageBackup::~BackupCreationWizardPageBackup() {
			delete ui;
		}

		void BackupCreationWizardPageBackup::initializePage() {
			QString const password = field("edtPassword").toString();
			QString const backup = m_identityBackup.toBackupString(password);
			ui->edtBackupString->setText(backup.toUpper());
			ui->edtBackupString->selectAll();
		}
	}
}
