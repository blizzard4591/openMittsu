#include "ContactEditWizard.h"
#include "ui_contacteditwizard.h"

#include "ContactRegistry.h"
#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"

ContactEditWizard::ContactEditWizard(ClientConfiguration* clientConfiguration, ProtocolClient* protocolClient, IdentityContact* preChosenContact, QWidget *parent) : QWizard(parent), ui(new Ui::ContactEditWizard), clientConfiguration(clientConfiguration), protocolClient(protocolClient), haveFinished(false), preChosenContact(preChosenContact) {
    ui->setupUi(this);
	setOption(QWizard::NoBackButtonOnLastPage, true);

	OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

	contactEditWizardPageInfo = new ContactEditWizardPageSelectContact(clientConfiguration, this);
	this->addPage(contactEditWizardPageInfo);
	contactEditWizardPageDone = new ContactEditWizardPageDone(protocolClient, clientConfiguration, this);
	this->addPage(contactEditWizardPageDone);
}

ContactEditWizard::~ContactEditWizard() {
    delete ui;
}

void ContactEditWizard::pageNextOnClick(int pageId) {
	//
}