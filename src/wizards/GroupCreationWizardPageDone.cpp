#include "src/wizards/GroupCreationWizardPageDone.h"
#include "ui_groupcreationwizardpagedone.h"

#include <QMessageBox>
#include <QSet>

#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include "src/utility/MakeUnique.h"

#include "src/widgets/ContactListWidgetItem.h"

namespace openmittsu {
	namespace wizards {

		GroupCreationWizardPageDone::GroupCreationWizardPageDone(QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::GroupCreationWizardPageDone>()) {
			m_ui->setupUi(this);
		}

		GroupCreationWizardPageDone::~GroupCreationWizardPageDone() {
			//
		}
	}
}
