#include "src/wizards/GroupCreationWizard.h"
#include "ui_groupcreationwizard.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {
		GroupCreationWizard::GroupCreationWizard(QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::GroupCreationWizard>()) {
			m_ui->setupUi(this);
			setOption(QWizard::NoBackButtonOnLastPage, true);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			m_groupCreationWizardPageInfo = new GroupCreationWizardPageInfo(knownIdentitiesWithNicknamesExcludingSelfContactId, std::move(groupCreationProcessor), this);
			this->addPage(m_groupCreationWizardPageInfo);
			m_groupCreationWizardPageDone = new GroupCreationWizardPageDone(this);
			this->addPage(m_groupCreationWizardPageDone);
		}

		GroupCreationWizard::~GroupCreationWizard() {
			// Ownership is with QObject/Parent
			m_groupCreationWizardPageDone = nullptr;
			m_groupCreationWizardPageInfo = nullptr;
		}

		void GroupCreationWizard::pageNextOnClick(int pageId) {
			//
		}
	}
}
