#include "LoadBackupWizard.h"
#include "ui_loadbackupwizard.h"

#include <QMessageBox>
#include "ClientConfiguration.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"

LoadBackupWizard::LoadBackupWizard(QWidget *parent) : QWizard(parent), ui(new Ui::LoadBackupWizard), loadBackupWizardPageData(nullptr), loadBackupWizardPageSaveIdentity(nullptr) {
    ui->setupUi(this);

	OPENMITTSU_CONNECT(this, currentIdChanged(int), this, pageNextOnClick(int));

	loadBackupWizardPageData = new LoadBackupWizardPageData(this);
	this->addPage(loadBackupWizardPageData);

	loadBackupWizardPageSaveIdentity = new LoadBackupWizardPageSaveIdentity(this);
	this->addPage(loadBackupWizardPageSaveIdentity);

	loadBackupWizardPageDone = new LoadBackupWizardPageDone(this);
	this->addPage(loadBackupWizardPageDone);

	setOption(QWizard::NoBackButtonOnLastPage, true);
}

LoadBackupWizard::~LoadBackupWizard() {
    delete ui;
}

void LoadBackupWizard::pageNextOnClick(int pageId) {
	if (page(pageId) == loadBackupWizardPageSaveIdentity) {
		QString const backupString = field("edtBackupString").toString();
		QString const passwordString = field("edtPassword").toString();

		try {
			ClientConfiguration::BackupData data(ClientConfiguration::fromBackup(backupString, passwordString));
		} catch (IllegalArgumentException&) {
			back();
			QMessageBox::warning(this, "Incorrect Password or Backup", "Either the password is incorrect or you mistyped the Backup string!");
		}
	} else if (page(pageId) == loadBackupWizardPageDone) {
		QString const backupString = field("edtBackupString").toString();
		QString const passwordString = field("edtPassword").toString();
		if (!ClientConfiguration::toFile(field("edtFilename").toString(), backupString, passwordString)) {
			back();
			QMessageBox::warning(this, "Error while writing file", "Is the selected file writable or is it still opened in other applications?");
		}
	}
}