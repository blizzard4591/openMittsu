#include "src/wizards/ContactEditWizard.h"
#include "ui_contacteditwizard.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {

		ContactEditWizard::ContactEditWizard(std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const& contactDataProvider, QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::ContactEditWizard>()), m_contactDataProvider(contactDataProvider), m_haveFinished(false) {
			m_ui->setupUi(this);
			setOption(QWizard::NoBackButtonOnLastPage, true);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			m_contactEditWizardPageInfo = new ContactEditWizardPageSelectContact(contactDataProvider, this);
			this->addPage(m_contactEditWizardPageInfo);
			m_contactEditWizardPageDone = new ContactEditWizardPageDone(this);
			this->addPage(m_contactEditWizardPageDone);
		}

		ContactEditWizard::~ContactEditWizard() {
			// Ownership is with QObject/Parent
			m_contactEditWizardPageDone = nullptr;
			m_contactEditWizardPageInfo = nullptr;
		}

		void ContactEditWizard::pageNextOnClick(int pageId) {
			//
		}

	}
}
