#ifndef LOADBACKUPWIZARD_H
#define LOADBACKUPWIZARD_H

#include <QWizard>

#include "LoadBackupWizardPageData.h"
#include "LoadBackupWizardPageSaveIdentity.h"
#include "LoadBackupWizardPageDone.h"

namespace Ui {
class LoadBackupWizard;
}

class LoadBackupWizard : public QWizard {
    Q_OBJECT

public:
	explicit LoadBackupWizard(QWidget *parent = 0);
    ~LoadBackupWizard();
public slots:
	void pageNextOnClick(int pageId);
private:
    Ui::LoadBackupWizard *ui;

	LoadBackupWizardPageData* loadBackupWizardPageData;
	LoadBackupWizardPageSaveIdentity* loadBackupWizardPageSaveIdentity;
	LoadBackupWizardPageDone* loadBackupWizardPageDone;
};

#endif // LOADBACKUPWIZARD_H
