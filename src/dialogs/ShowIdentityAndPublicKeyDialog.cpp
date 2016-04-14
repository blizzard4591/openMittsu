#include "ShowIdentityAndPublicKeyDialog.h"
#include "ui_showidentityandpublickeydialog.h"

ShowIdentityAndPublicKeyDialog::ShowIdentityAndPublicKeyDialog(ClientConfiguration* clientConfiguration, QWidget *parent) : QDialog(parent), ui(new Ui::ShowIdentityAndPublicKeyDialog) {
    ui->setupUi(this);

	QString const identityString(clientConfiguration->getClientIdentity().toQString());
	QString const publicKeyString(clientConfiguration->getClientLongTermKeyPair().getPublicKey().toHex());

	ui->edtIdentity->setText(identityString);
	ui->edtPublicKey->setText(publicKeyString);
	ui->edtContact->setText(QString("%1 : %2 : \"Your Name here\"").arg(identityString).arg(publicKeyString));
}

ShowIdentityAndPublicKeyDialog::~ShowIdentityAndPublicKeyDialog() {
    delete ui;
}
