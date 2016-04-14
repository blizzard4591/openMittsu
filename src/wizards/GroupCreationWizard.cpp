#include "GroupCreationWizard.h"
#include "ui_groupcreationwizard.h"

#include "ContactRegistry.h"
#include "ContactListWidgetItem.h"
#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"

GroupCreationWizard::GroupCreationWizard(ClientConfiguration* clientConfiguration, ProtocolClient* protocolClient, QWidget *parent) : QWizard(parent), ui(new Ui::GroupCreationWizard), clientConfiguration(clientConfiguration), protocolClient(protocolClient), haveFinished(false) {
    ui->setupUi(this);
	setOption(QWizard::NoBackButtonOnLastPage, true);

	OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

	groupCreationWizardPageInfo = new GroupCreationWizardPageInfo(clientConfiguration, this);
	this->addPage(groupCreationWizardPageInfo);
	groupCreationWizardPageDone = new GroupCreationWizardPageDone(protocolClient, clientConfiguration, groupCreationWizardPageInfo->getSelectedGroupMembersWidgetPointer(), this);
	this->addPage(groupCreationWizardPageDone);
}

GroupCreationWizard::~GroupCreationWizard() {
    delete ui;
}

void GroupCreationWizard::pageNextOnClick(int pageId) {
	//
}