#include "ContactAddDialog.h"
#include "ui_contactadddialog.h"

#include <QRegularExpression>

ContactAddDialog::ContactAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ContactAddDialog) {
    ui->setupUi(this);

	identityValidator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[A-Z0-9*][A-Z0-9]{7}")), ui->edtIdentity);
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
