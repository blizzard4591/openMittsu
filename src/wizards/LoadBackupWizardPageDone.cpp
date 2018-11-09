#include "src/wizards/LoadBackupWizardPageDone.h"
#include "ui_LoadBackupWizardPageDone.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageDone::LoadBackupWizardPageDone(QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageDone>()) {
			m_ui->setupUi(this);
		}

		LoadBackupWizardPageDone::~LoadBackupWizardPageDone() {
			//
		}

	}
}
