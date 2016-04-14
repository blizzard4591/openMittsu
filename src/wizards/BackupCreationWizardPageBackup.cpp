#include "BackupCreationWizardPageBackup.h"
#include "ui_backupcreationwizardpagebackup.h"

BackupCreationWizardPageBackup::BackupCreationWizardPageBackup(ClientConfiguration* clientConfiguration, QWidget *parent) : QWizardPage(parent), ui(new Ui::BackupCreationWizardPageBackup), clientConfiguration(clientConfiguration) {
    ui->setupUi(this);

	registerField("edtBackupString*", ui->edtBackupString);
}

BackupCreationWizardPageBackup::~BackupCreationWizardPageBackup() {
    delete ui;
}

void BackupCreationWizardPageBackup::initializePage() {
	QString const password = field("edtPassword").toString();
	QString backup = clientConfiguration->toBackup(password);
	ui->edtBackupString->setText(backup.toUpper());
	ui->edtBackupString->selectAll();
}