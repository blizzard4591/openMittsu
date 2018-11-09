#include "src/dialogs/ShowIdentityAndPublicKeyDialog.h"

#include "ui_showidentityandpublickeydialog.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace dialogs {
		ShowIdentityAndPublicKeyDialog::ShowIdentityAndPublicKeyDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent) : QDialog(parent), m_ui(std::make_unique<Ui::ShowIdentityAndPublicKeyDialog>()) {
			m_ui->setupUi(this);

			QString const identityString(selfContactId.toQString());
			QString const publicKeyString(selfPublicKey.getPublicKey().toHex());

			m_ui->edtIdentity->setText(identityString);
			m_ui->edtPublicKey->setText(publicKeyString);
			m_ui->edtContact->setText(QString("%1 : %2 : \"Your Name here\"").arg(identityString).arg(publicKeyString));
		}

		ShowIdentityAndPublicKeyDialog::~ShowIdentityAndPublicKeyDialog() {
			//
		}
	}
}
