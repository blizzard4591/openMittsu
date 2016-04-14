#ifndef GROUPCREATIONWIZARDPAGEINFO_H
#define GROUPCREATIONWIZARDPAGEINFO_H

#include <QWizardPage>
#include <QListWidget>
#include <QRegExpValidator>
#include "ClientConfiguration.h"

namespace Ui {
class GroupCreationWizardPageInfo;
}

class GroupCreationWizardPageInfo : public QWizardPage
{
    Q_OBJECT

public:
	explicit GroupCreationWizardPageInfo(ClientConfiguration* clientConfiguration, QWidget *parent = 0);
    ~GroupCreationWizardPageInfo();

	bool isComplete() const override;
	QListWidget const* getSelectedGroupMembersWidgetPointer() const;
public slots:
	void onListWidgetItemSelectionChanged();
private:
    Ui::GroupCreationWizardPageInfo *ui;
	ClientConfiguration* const clientConfiguration;

	QRegExpValidator* nameValidator;
	bool isSelectionOkay;
};

#endif // GROUPCREATIONWIZARDPAGEINFO_H
