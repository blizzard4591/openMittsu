#ifndef GROUPCREATIONWIZARD_H
#define GROUPCREATIONWIZARD_H

#include <QWizard>
#include "ClientConfiguration.h"
#include "ProtocolClient.h"

#include "GroupCreationWizardPageInfo.h"
#include "GroupCreationWizardPageDone.h"

namespace Ui {
class GroupCreationWizard;
}

class GroupCreationWizard : public QWizard
{
    Q_OBJECT

public:
    explicit GroupCreationWizard(ClientConfiguration* clientConfiguration, ProtocolClient* protocolClient, QWidget *parent = 0);
    ~GroupCreationWizard();
public slots:
	void pageNextOnClick(int pageId);
private:
    Ui::GroupCreationWizard *ui;
	ClientConfiguration* const clientConfiguration;
	ProtocolClient* const protocolClient;

	GroupCreationWizardPageInfo* groupCreationWizardPageInfo;
	GroupCreationWizardPageDone* groupCreationWizardPageDone;

	bool haveFinished;
};

#endif // GROUPCREATIONWIZARD_H
