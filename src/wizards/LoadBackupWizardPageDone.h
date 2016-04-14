#ifndef LOADBACKUPWIZARDPAGEDONE_H
#define LOADBACKUPWIZARDPAGEDONE_H

#include <QWizardPage>

namespace Ui {
class LoadBackupWizardPageDone;
}

class LoadBackupWizardPageDone : public QWizardPage
{
    Q_OBJECT

public:
    explicit LoadBackupWizardPageDone(QWidget *parent = 0);
    ~LoadBackupWizardPageDone();

private:
    Ui::LoadBackupWizardPageDone *ui;
};

#endif // LOADBACKUPWIZARDPAGEDONE_H
