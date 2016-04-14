#include "BackupCreationWizardPagePassword.h"
#include "ui_backupcreationwizardpagepassword.h"

#include <QRegExp>

BackupCreationWizardPagePassword::BackupCreationWizardPagePassword(QWidget *parent) : QWizardPage(parent), ui(new Ui::BackupCreationWizardPagePassword) {
    ui->setupUi(this);

	// At least one character
	passwordValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), ui->edtPassword);
	ui->edtPassword->setValidator(passwordValidator);
	ui->edtPassword->setEchoMode(QLineEdit::EchoMode::Password);

	registerField("edtPassword*", ui->edtPassword);
}

BackupCreationWizardPagePassword::~BackupCreationWizardPagePassword() {
    delete ui;
	// Ownership is with ui->edtPassword
	passwordValidator = nullptr;
}