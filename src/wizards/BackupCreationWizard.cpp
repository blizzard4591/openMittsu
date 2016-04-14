#include "BackupCreationWizard.h"
#include "ui_backupcreationwizard.h"

#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"

BackupCreationWizard::BackupCreationWizard(ClientConfiguration* clientConfiguration, QWidget *parent) : QWizard(parent), ui(new Ui::BackupCreationWizard), clientConfiguration(clientConfiguration), backupCreationWizardPagePassword(nullptr), backupCreationWizardPageBackup(nullptr) {
    ui->setupUi(this);

	OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

	backupCreationWizardPagePassword = new BackupCreationWizardPagePassword(this);
	this->addPage(backupCreationWizardPagePassword);

	backupCreationWizardPageBackup = new BackupCreationWizardPageBackup(clientConfiguration, this);
	this->addPage(backupCreationWizardPageBackup);

	setOption(QWizard::NoBackButtonOnLastPage, true);
}

BackupCreationWizard::~BackupCreationWizard() {
    delete ui;
}

void BackupCreationWizard::pageNextOnClick(int pageId) {
	//
}