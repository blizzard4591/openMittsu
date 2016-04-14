#include "FingerprintDialog.h"
#include "ui_fingerprintdialog.h"

#include "widgets/QrWidget.h"

FingerprintDialog::FingerprintDialog(ClientConfiguration* clientConfiguration, QWidget *parent) : QDialog(parent), ui(new Ui::FingerprintDialog) {
    ui->setupUi(this);

	QString const identityString(QString::fromStdString(clientConfiguration->getClientIdentity().toString()));
	QString const fingerprint(clientConfiguration->getClientLongTermKeyPair().getFingerprint().toHex());
	QString const qrData = QString("3mid:%1,%2").arg(identityString).arg(QString(clientConfiguration->getClientLongTermKeyPair().getPublicKey().toHex()).toLower());

	ui->edtId->setText(identityString);
	ui->edtFingerprint->setText(fingerprint);
	ui->qrWidget->setQrDataString(qrData);
}

FingerprintDialog::~FingerprintDialog() {
    delete ui;
}
