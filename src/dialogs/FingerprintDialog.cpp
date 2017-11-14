#include "src/dialogs/FingerprintDialog.h"
#include "ui_fingerprintdialog.h"

#include "src/widgets/QrWidget.h"

namespace openmittsu {
	namespace dialogs {

		FingerprintDialog::FingerprintDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent) : QDialog(parent), ui(new Ui::FingerprintDialog) {
			ui->setupUi(this);

			QString const identityString(QString::fromStdString(selfContactId.toString()));
			QString const fingerprint(selfPublicKey.getFingerprint().toHex());
			QString const qrData = QString("3mid:%1,%2").arg(identityString).arg(QString(selfPublicKey.getPublicKey().toHex()).toLower());

			ui->edtId->setText(identityString);
			ui->edtFingerprint->setText(fingerprint);
			ui->qrWidget->setQrDataString(qrData);
		}

		FingerprintDialog::~FingerprintDialog() {
			delete ui;
		}

	}
}
