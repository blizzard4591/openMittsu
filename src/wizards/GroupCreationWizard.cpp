#include "src/wizards/GroupCreationWizard.h"
#include "ui_groupcreationwizard.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {
		GroupCreationWizard::GroupCreationWizard(QHash<openmittsu::protocol::ContactId, QString> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent) : QWizard(parent), ui(new Ui::GroupCreationWizard) {
			ui->setupUi(this);
			setOption(QWizard::NoBackButtonOnLastPage, true);

			OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

			groupCreationWizardPageInfo = new GroupCreationWizardPageInfo(knownIdentitiesWithNicknamesExcludingSelfContactId, std::move(groupCreationProcessor), this);
			this->addPage(groupCreationWizardPageInfo);
			groupCreationWizardPageDone = new GroupCreationWizardPageDone(this);
			this->addPage(groupCreationWizardPageDone);
		}

		GroupCreationWizard::~GroupCreationWizard() {
			delete ui;
		}

		void GroupCreationWizard::pageNextOnClick(int pageId) {
			//
		}
	}
}
