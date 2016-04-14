#include "ContactAddDialog.h"
#include "ui_contactadddialog.h"

#include <QRegExp>

ContactAddDialog::ContactAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ContactAddDialog) {
    ui->setupUi(this);

	identityValidator = new QRegExpValidator(QRegExp("[A-Z0-9]{8}", Qt::CaseSensitive, QRegExp::RegExp2), ui->edtIdentity);
	ui->edtIdentity->setValidator(identityValidator);
}

ContactAddDialog::~ContactAddDialog() {
    delete ui;
}

QString ContactAddDialog::getIdentity() const {
	return ui->edtIdentity->text();
}

QString ContactAddDialog::getNickname() const {
	return ui->edtNickname->text();
}