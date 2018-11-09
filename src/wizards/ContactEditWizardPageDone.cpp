#include "src/wizards/ContactEditWizardPageDone.h"
#include "ui_contacteditwizardpagedone.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace wizards {

		ContactEditWizardPageDone::ContactEditWizardPageDone(QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::ContactEditWizardPageDone>()) {
			m_ui->setupUi(this);
		}

		ContactEditWizardPageDone::~ContactEditWizardPageDone() {
			//
		}

		void ContactEditWizardPageDone::initializePage() {
			//
		}

	}
}
