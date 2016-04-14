#ifndef GROUPCREATIONWIZARDPAGEDONE_H
#define GROUPCREATIONWIZARDPAGEDONE_H

#include <QWizardPage>
#include <QListWidget>
#include "ProtocolClient.h"

namespace Ui {
class GroupCreationWizardPageDone;
}

class GroupCreationWizardPageDone : public QWizardPage {
    Q_OBJECT

public:
	explicit GroupCreationWizardPageDone(ProtocolClient* protocolClient, ClientConfiguration* clientConfiguration, QListWidget const* selectedGroupMembersWidget, QWidget *parent = 0);
    ~GroupCreationWizardPageDone();

	virtual void initializePage();
private:
    Ui::GroupCreationWizardPageDone *ui;
	ProtocolClient* const protocolClient;
	ClientConfiguration* const clientConfiguration;
	QListWidget const*const groupMemberWidget;
};

#endif // GROUPCREATIONWIZARDPAGEDONE_H
