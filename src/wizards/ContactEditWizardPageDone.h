#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_

#include <QWizardPage>
#include "ProtocolClient.h"

namespace Ui {
class ContactEditWizardPageDone;
}

class ContactEditWizardPageDone : public QWizardPage {
    Q_OBJECT

public:
	explicit ContactEditWizardPageDone(ProtocolClient* protocolClient, ClientConfiguration* clientConfiguration, QWidget *parent = 0);
	~ContactEditWizardPageDone();

	virtual void initializePage();
private:
	Ui::ContactEditWizardPageDone *ui;
	ProtocolClient* const protocolClient;
	ClientConfiguration* const clientConfiguration;
};

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_
