#include "GroupCreationWizardPageDone.h"
#include "ui_groupcreationwizardpagedone.h"

#include "ContactRegistry.h"
#include "ContactListWidgetItem.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"

#include <QSet>

GroupCreationWizardPageDone::GroupCreationWizardPageDone(ProtocolClient* protocolClient, ClientConfiguration* clientConfiguration, QListWidget const* selectedGroupMembersWidget, QWidget *parent) : QWizardPage(parent), ui(new Ui::GroupCreationWizardPageDone), protocolClient(protocolClient), clientConfiguration(clientConfiguration), groupMemberWidget(selectedGroupMembersWidget) {
    ui->setupUi(this);
}

GroupCreationWizardPageDone::~GroupCreationWizardPageDone() {
    delete ui;
}

void GroupCreationWizardPageDone::initializePage() {
	// all data in the Info section was given, now create the group
	// Generate new random ID as group identifier
	GroupId newGroupId = GroupId::createRandomGroupId(clientConfiguration->getClientIdentity());
	
	QList<QListWidgetItem*> selectedItems = groupMemberWidget->selectedItems();
	QList<QListWidgetItem*>::const_iterator it = selectedItems.constBegin();
	QList<QListWidgetItem*>::const_iterator end = selectedItems.constEnd();
	
	QSet<ContactId> groupMembers;
	groupMembers.insert(clientConfiguration->getClientIdentity());

	for (; it != end; ++it) {
		ContactListWidgetItem* clwi = dynamic_cast<ContactListWidgetItem*>(*it);
		IdentityContact* ic = dynamic_cast<IdentityContact*>(clwi->getContact());
		groupMembers.insert(ic->getContactId());
	}

	// Send messages to members
	QMetaObject::invokeMethod(protocolClient, "sendGroupSetup", Q_ARG(GroupId, newGroupId), Q_ARG(QSet<ContactId>, groupMembers), Q_ARG(QString, field("edtName").toString()));

	GroupContact* groupContact = new GroupContact(newGroupId);
	groupContact->addMember(groupMembers);
	groupContact->setGroupName(field("edtName").toString());

	ContactRegistry::getInstance()->addGroup(groupContact);
	//QThread::msleep(100);
	//QMetaObject::invokeMethod(protocolClient, "sendGroupTitle", Q_ARG(quint64, newGroupId), Q_ARG(QString const&, field("edtName").toString()));
}