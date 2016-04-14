#include "GroupCreationWizardPageInfo.h"
#include "ui_groupcreationwizardpageinfo.h"

#include "ContactRegistry.h"
#include "ContactListWidgetItem.h"
#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"


GroupCreationWizardPageInfo::GroupCreationWizardPageInfo(ClientConfiguration* clientConfiguration, QWidget *parent) : QWizardPage(parent), ui(new Ui::GroupCreationWizardPageInfo), clientConfiguration(clientConfiguration) {
    ui->setupUi(this);

	nameValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), ui->edtName);
	ui->edtName->setValidator(nameValidator);

	ui->listWidget->clear();
	ui->listWidget->setSelectionMode(QListWidget::SelectionMode::MultiSelection);

	OPENMITTSU_CONNECT(ui->listWidget, itemSelectionChanged(), this, onListWidgetItemSelectionChanged());

	// Load all Identities
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();
	QList<ContactId> knownIdentities = contactRegistry->getIdentities();
	QList<ContactId>::const_iterator it = knownIdentities.constBegin();
	QList<ContactId>::const_iterator end = knownIdentities.constEnd();

	ContactId const selfIdentity = (clientConfiguration == nullptr) ? ContactId(0) : clientConfiguration->getClientIdentity();
	for (; it != end; ++it) {
		if (*it == selfIdentity) {
			continue;
		}
		ContactListWidgetItem* clwi = new ContactListWidgetItem(contactRegistry->getIdentity(*it), contactRegistry->getNickname(*it));
		ui->listWidget->addItem(clwi);
	}
	ui->listWidget->clearSelection();
	isSelectionOkay = false;

	registerField("edtName*", ui->edtName);
	registerField("listWidget*", ui->listWidget);
}

GroupCreationWizardPageInfo::~GroupCreationWizardPageInfo() {
    delete ui;
}

bool GroupCreationWizardPageInfo::isComplete() const {
	if (ui->edtName->hasAcceptableInput() && isSelectionOkay) {
		return true;
	}
	return false;
}

void GroupCreationWizardPageInfo::onListWidgetItemSelectionChanged() {
	if (ui->listWidget->selectedItems().size() > 0) {
		isSelectionOkay = true;
	} else {
		isSelectionOkay = false;
	}
	emit completeChanged();
}

QListWidget const* GroupCreationWizardPageInfo::getSelectedGroupMembersWidgetPointer() const {
	return ui->listWidget;
}