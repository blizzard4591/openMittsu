#include "LoadBackupWizardPageSaveIdentity.h"
#include "ui_loadbackupwizardpagesaveidentity.h"

#include "ClientConfiguration.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "utility/QObjectConnectionMacro.h"

#include <QFileDialog>
#include <QRegExp>
#include <QFile>

LoadBackupWizardPageSaveIdentity::LoadBackupWizardPageSaveIdentity(QWidget *parent) : QWizardPage(parent), ui(new Ui::LoadBackupWizardPageSaveIdentity) {
    ui->setupUi(this);

	OPENMITTSU_CONNECT(ui->btnPickSave, clicked(), this, btnPickSaveOnClick());

	fileNameValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), ui->edtFilename);
	ui->edtFilename->setValidator(fileNameValidator);

	registerField("edtFilename*", ui->edtFilename);
}

LoadBackupWizardPageSaveIdentity::~LoadBackupWizardPageSaveIdentity() {
    delete ui;
}

void LoadBackupWizardPageSaveIdentity::initializePage() {
	ui->edtFilename->setText("");
}

void LoadBackupWizardPageSaveIdentity::btnPickSaveOnClick() {
	QString const fileName = QFileDialog::getSaveFileName(this, "Save client configuration", "", "Client Configurations (*.ini)");
	if (fileName.isEmpty() || fileName.isNull()) {
		ui->edtFilename->setText("");
	} else {
		ui->edtFilename->setText(fileName);
	}
}