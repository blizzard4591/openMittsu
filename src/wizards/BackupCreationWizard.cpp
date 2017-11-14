#include "src/wizards/BackupCreationWizard.h"
#include "ui_backupcreationwizard.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {
		BackupCreationWizard::BackupCreationWizard(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent) : QWizard(parent), ui(new Ui::BackupCreationWizard), backupCreationWizardPagePassword(nullptr), backupCreationWizardPageBackup(nullptr) {
			ui->setupUi(this);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			backupCreationWizardPagePassword = new BackupCreationWizardPagePassword(this);
			this->addPage(backupCreationWizardPagePassword);

			backupCreationWizardPageBackup = new BackupCreationWizardPageBackup(identityBackup, this);
			this->addPage(backupCreationWizardPageBackup);

			setOption(QWizard::NoBackButtonOnLastPage, true);
		}

		BackupCreationWizard::~BackupCreationWizard() {
			delete ui;
		}

		void BackupCreationWizard::pageNextOnClick(int pageId) {
			//
		}
	}
}
