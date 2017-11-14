#include "src/wizards/ContactEditWizard.h"
#include "ui_contacteditwizard.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {

		ContactEditWizard::ContactEditWizard(std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const& contactDataProvider, QWidget* parent) : QWizard(parent), m_ui(new Ui::ContactEditWizard), m_contactDataProvider(contactDataProvider), m_haveFinished(false) {
			m_ui->setupUi(this);
			setOption(QWizard::NoBackButtonOnLastPage, true);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			m_contactEditWizardPageInfo = new ContactEditWizardPageSelectContact(contactDataProvider, this);
			this->addPage(m_contactEditWizardPageInfo);
			m_contactEditWizardPageDone = new ContactEditWizardPageDone(this);
			this->addPage(m_contactEditWizardPageDone);
		}

		ContactEditWizard::~ContactEditWizard() {
			delete m_ui;
		}

		void ContactEditWizard::pageNextOnClick(int pageId) {
			//
		}

	}
}
