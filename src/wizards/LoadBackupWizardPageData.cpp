#include "LoadBackupWizardPageData.h"
#include "ui_loadbackupwizardpagedata.h"

#include <QRegExp>
#include "ClientConfiguration.h"
#include "exceptions/IllegalArgumentException.h"

LoadBackupWizardPageData::LoadBackupWizardPageData(QWidget *parent) : QWizardPage(parent), ui(new Ui::LoadBackupWizardPageData) {
    ui->setupUi(this);

	// Suitable format
	backupStringValidator = new QRegExpValidator(QRegExp("^[A-Z2-7]{4}(?:-[A-Z2-7]{4})*$", Qt::CaseSensitive, QRegExp::RegExp2), ui->edtBackupString);
	ui->edtBackupString->setValidator(backupStringValidator);

	// At least one character
	passwordValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), ui->edtPassword);
	ui->edtPassword->setValidator(passwordValidator);
	ui->edtPassword->setEchoMode(QLineEdit::EchoMode::Password);

	registerField("edtPassword*", ui->edtPassword);
	registerField("edtBackupString*", ui->edtBackupString);
}

LoadBackupWizardPageData::~LoadBackupWizardPageData() {
    delete ui;
	// Ownership is with the lineEdits
	backupStringValidator = nullptr;
	passwordValidator = nullptr;
}