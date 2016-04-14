#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_

#include <QWizardPage>
#include "ClientConfiguration.h"

namespace Ui {
class BackupCreationWizardPageBackup;
}

class BackupCreationWizardPageBackup : public QWizardPage
{
    Q_OBJECT

public:
    explicit BackupCreationWizardPageBackup(ClientConfiguration* clientConfiguration, QWidget *parent = 0);
    ~BackupCreationWizardPageBackup();

	//bool isComplete() const override;
	virtual void initializePage() override;
private:
    Ui::BackupCreationWizardPageBackup *ui;
	ClientConfiguration* clientConfiguration;
};

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_
