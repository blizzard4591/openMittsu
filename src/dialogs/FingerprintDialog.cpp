#include "src/dialogs/FingerprintDialog.h"
#include "ui_fingerprintdialog.h"

#include "src/widgets/QrWidget.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace dialogs {

		FingerprintDialog::FingerprintDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent) : QDialog(parent), m_ui(std::make_unique<Ui::FingerprintDialog>()) {
			m_ui->setupUi(this);

			QString const identityString(QString::fromStdString(selfContactId.toString()));
			QString const fingerprint(selfPublicKey.getFingerprint().toHex());
			QString const qrData = QString("3mid:%1,%2").arg(identityString).arg(QString(selfPublicKey.getPublicKey().toHex()).toLower());

			m_ui->edtId->setText(identityString);
			m_ui->edtFingerprint->setText(fingerprint);
			m_ui->qrWidget->setQrDataString(qrData);
		}

		FingerprintDialog::~FingerprintDialog() {
			//
		}

	}
}
