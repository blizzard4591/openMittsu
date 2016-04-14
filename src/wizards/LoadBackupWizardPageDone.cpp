#include "LoadBackupWizardPageDone.h"
#include "ui_loadbackupwizardpagedone.h"

LoadBackupWizardPageDone::LoadBackupWizardPageDone(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::LoadBackupWizardPageDone)
{
    ui->setupUi(this);
}

LoadBackupWizardPageDone::~LoadBackupWizardPageDone()
{
    delete ui;
}
