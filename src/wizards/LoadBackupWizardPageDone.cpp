#include "src/wizards/LoadBackupWizardPageDone.h"
#include "ui_LoadBackupWizardPageDone.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageDone::LoadBackupWizardPageDone(QWidget* parent) :
			QWizardPage(parent),
			m_ui(new Ui::LoadBackupWizardPageDone) {
			m_ui->setupUi(this);
		}

		LoadBackupWizardPageDone::~LoadBackupWizardPageDone() {
			delete m_ui;
		}

	}
}
