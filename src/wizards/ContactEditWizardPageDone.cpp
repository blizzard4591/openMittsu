#include "src/wizards/ContactEditWizardPageDone.h"
#include "ui_contacteditwizardpagedone.h"

namespace openmittsu {
	namespace wizards {

		ContactEditWizardPageDone::ContactEditWizardPageDone(QWidget* parent) : QWizardPage(parent), m_ui(new Ui::ContactEditWizardPageDone) {
			m_ui->setupUi(this);
		}

		ContactEditWizardPageDone::~ContactEditWizardPageDone() {
			delete m_ui;
		}

		void ContactEditWizardPageDone::initializePage() {
			//
		}

	}
}
