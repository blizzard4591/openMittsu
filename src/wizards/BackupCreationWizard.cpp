#include "src/wizards/BackupCreationWizard.h"
#include "ui_backupcreationwizard.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {
		BackupCreationWizard::BackupCreationWizard(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::BackupCreationWizard>()), m_backupCreationWizardPagePassword(nullptr), m_backupCreationWizardPageBackup(nullptr) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			m_backupCreationWizardPagePassword = new BackupCreationWizardPagePassword(this);
			this->addPage(m_backupCreationWizardPagePassword);

			m_backupCreationWizardPageBackup = new BackupCreationWizardPageBackup(identityBackup, this);
			this->addPage(m_backupCreationWizardPageBackup);

			setOption(QWizard::NoBackButtonOnLastPage, true);
		}

		BackupCreationWizard::~BackupCreationWizard() {
			//
		}

		void BackupCreationWizard::pageNextOnClick(int pageId) {
			//
		}
	}
}
