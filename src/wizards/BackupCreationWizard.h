#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_

#include <QWizard>

#include "ClientConfiguration.h"
#include "BackupCreationWizardPageBackup.h"
#include "BackupCreationWizardPagePassword.h"

namespace Ui {
class BackupCreationWizard;
}

class BackupCreationWizard : public QWizard
{
    Q_OBJECT

public:
	explicit BackupCreationWizard(ClientConfiguration* clientConfiguration, QWidget *parent = 0);
    ~BackupCreationWizard();
public slots:
	void pageNextOnClick(int pageId);
private:
    Ui::BackupCreationWizard *ui;
	ClientConfiguration* const clientConfiguration;

	BackupCreationWizardPagePassword* backupCreationWizardPagePassword;
	BackupCreationWizardPageBackup* backupCreationWizardPageBackup;
};

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
