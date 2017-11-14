#ifndef OPENMITTSU_DIALOGS_SHOWIDENTITYANDPUBLICKEYDIALOG_H_
#define OPENMITTSU_DIALOGS_SHOWIDENTITYANDPUBLICKEYDIALOG_H_

#include <QDialog>

#include "src/protocol/ContactId.h"
#include "src/crypto/PublicKey.h"

namespace Ui {
class ShowIdentityAndPublicKeyDialog;
}

namespace openmittsu {
	namespace dialogs {
		class ShowIdentityAndPublicKeyDialog : public QDialog {
			Q_OBJECT
		public:
			explicit ShowIdentityAndPublicKeyDialog(openmittsu::protocol::ContactId const& selfContactId, openmittsu::crypto::PublicKey const& selfPublicKey, QWidget* parent = nullptr);
			~ShowIdentityAndPublicKeyDialog();

		private:
			Ui::ShowIdentityAndPublicKeyDialog *ui;
		};
	}
}

#endif // OPENMITTSU_DIALOGS_SHOWIDENTITYANDPUBLICKEYDIALOG_H_
