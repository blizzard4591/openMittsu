#include "src/dialogs/ContactEditDialog.h"
#include "ui_ContactEditDialog.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace dialogs {

		ContactEditDialog::ContactEditDialog(QString const& identity, QString const& pubKey, QString const& nickname, QWidget *parent) : QDialog(parent), m_ui(std::make_unique<Ui::ContactEditDialog>()) {
			m_ui->setupUi(this);
			m_ui->edtIdentity->setText(identity);
			m_ui->edtPubKey->setText(pubKey);
			m_ui->edtNickname->setText(nickname);
		}


		ContactEditDialog::~ContactEditDialog() {
			//
		}

		QString ContactEditDialog::getNickname() const {
			return m_ui->edtNickname->text();
		}

	}
}
