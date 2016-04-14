#include "ContactEditWizardPageDone.h"
#include "ui_contacteditwizardpagedone.h"

#include "ContactRegistry.h"
#include "ContactListWidgetItem.h"

ContactEditWizardPageDone::ContactEditWizardPageDone(ProtocolClient* protocolClient, ClientConfiguration* clientConfiguration, QWidget *parent) : QWizardPage(parent), ui(new Ui::ContactEditWizardPageDone), protocolClient(protocolClient), clientConfiguration(clientConfiguration) {
    ui->setupUi(this);
}

ContactEditWizardPageDone::~ContactEditWizardPageDone() {
    delete ui;
}

void ContactEditWizardPageDone::initializePage() {
	// all data in the Info section was given, now create the group
	// Generate new random ID as group identifier
	quint64 newGroupId = GroupContact::generateRandomGroupId();
	GroupContact* gc = new GroupContact(GroupId(clientConfiguration->getClientIdentity(), newGroupId));
	gc->setGroupName(field("edtName").toString());
	gc->addMember(clientConfiguration->getClientIdentity());

	// Send messages to members
	QMetaObject::invokeMethod(protocolClient, "sendGroupCreation", Q_ARG(quint64, newGroupId), Q_ARG(bool, true));
	QThread::msleep(100);
	QMetaObject::invokeMethod(protocolClient, "sendGroupTitle", Q_ARG(quint64, newGroupId), Q_ARG(QString const&, field("edtName").toString()));
}