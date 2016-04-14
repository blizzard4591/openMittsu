#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_

#include <QWizard>
#include "ClientConfiguration.h"
#include "ProtocolClient.h"
#include "IdentityContact.h"

#include "ContactEditWizardPageSelectContact.h"
#include "ContactEditWizardPageDone.h"

namespace Ui {
class ContactEditWizard;
}

class ContactEditWizard : public QWizard
{
    Q_OBJECT

public:
	explicit ContactEditWizard(ClientConfiguration* clientConfiguration, ProtocolClient* protocolClient, IdentityContact* preChosenContact = nullptr , QWidget *parent = nullptr);
	~ContactEditWizard();
public slots:
	void pageNextOnClick(int pageId);
private:
	Ui::ContactEditWizard *ui;
	ClientConfiguration* const clientConfiguration;
	ProtocolClient* const protocolClient;

	ContactEditWizardPageSelectContact* contactEditWizardPageInfo;
	ContactEditWizardPageDone* contactEditWizardPageDone;

	bool haveFinished;
	IdentityContact* preChosenContact;
};

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_
