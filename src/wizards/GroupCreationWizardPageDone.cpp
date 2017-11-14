#include "src/wizards/GroupCreationWizardPageDone.h"
#include "ui_groupcreationwizardpagedone.h"

#include "src/widgets/ContactListWidgetItem.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/GroupId.h"

#include <QMessageBox>
#include <QSet>

namespace openmittsu {
	namespace wizards {

		GroupCreationWizardPageDone::GroupCreationWizardPageDone(QWidget *parent) : QWizardPage(parent), m_ui(new Ui::GroupCreationWizardPageDone) {
			m_ui->setupUi(this);
		}

		GroupCreationWizardPageDone::~GroupCreationWizardPageDone() {
			delete m_ui;
		}
	}
}
