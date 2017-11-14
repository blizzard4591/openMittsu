#include "src/dialogs/ShowIdentityAndPublicKeyDialog.h"

#include "ui_showidentityandpublickeydialog.h"

namespace openmittsu {
	namespace dialogs {
		ShowIdentityAndPublicKeyDialog::ShowIdentityAndPublicKeyDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent) : QDialog(parent), ui(new Ui::ShowIdentityAndPublicKeyDialog) {
			ui->setupUi(this);

			QString const identityString(selfContactId.toQString());
			QString const publicKeyString(selfPublicKey.getPublicKey().toHex());

			ui->edtIdentity->setText(identityString);
			ui->edtPublicKey->setText(publicKeyString);
			ui->edtContact->setText(QString("%1 : %2 : \"Your Name here\"").arg(identityString).arg(publicKeyString));
		}

		ShowIdentityAndPublicKeyDialog::~ShowIdentityAndPublicKeyDialog() {
			delete ui;
		}
	}
}
