#ifndef OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_
#define OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_

#include <QDialog>

#include "src/protocol/ContactId.h"
#include "src/crypto/PublicKey.h"

namespace Ui {
class FingerprintDialog;
}

namespace openmittsu {
	namespace dialogs {
		class FingerprintDialog : public QDialog {
			Q_OBJECT
		public:
			explicit FingerprintDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent = nullptr);
			virtual ~FingerprintDialog();

		private:
			Ui::FingerprintDialog* ui;
		};
	}
}

#endif // OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_
