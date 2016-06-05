#include "ContactEditDialog.h"
#include "ui_ContactEditDialog.h"

ContactEditDialog::ContactEditDialog(QString const& identity, QString const& pubKey, QString const& nickname, QWidget *parent) : QDialog(parent), ui(new Ui::ContactEditDialog) {
    ui->setupUi(this);
	ui->edtIdentity->setText(identity);
	ui->edtPubKey->setText(pubKey);
	ui->edtNickname->setText(nickname);
}


ContactEditDialog::~ContactEditDialog() {
    delete ui;
}

QString ContactEditDialog::getNickname() const {
	return ui->edtNickname->text();
}
