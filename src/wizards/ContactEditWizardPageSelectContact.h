#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_

#include <QWizardPage>
#include <QListWidget>
#include <QRegExpValidator>
#include "ClientConfiguration.h"

namespace Ui {
	class ContactEditWizardPageSelectContact;
}

class ContactEditWizardPageSelectContact : public QWizardPage
{
    Q_OBJECT

public:
	explicit ContactEditWizardPageSelectContact(ClientConfiguration* clientConfiguration, QWidget *parent = 0);
	~ContactEditWizardPageSelectContact();

	bool isComplete() const override;
	QListWidget const* getSelectedGroupMembersWidgetPointer() const;
public slots:
	void onListWidgetItemSelectionChanged();
private:
	Ui::ContactEditWizardPageSelectContact *ui;
	ClientConfiguration* const clientConfiguration;

	QRegExpValidator* nameValidator;
	bool isSelectionOkay;
};

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_
