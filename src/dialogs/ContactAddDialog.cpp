#include "src/dialogs/ContactAddDialog.h"

#include <QRegularExpression>

#include "ui_contactadddialog.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace dialogs {

		ContactAddDialog::ContactAddDialog(QWidget *parent) : QDialog(parent), m_ui(std::make_unique<Ui::ContactAddDialog>()) {
			m_ui->setupUi(this);

			m_identityValidator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[a-zA-Z0-9*][a-zA-Z0-9]{7}")), m_ui->edtIdentity);
			m_ui->edtIdentity->setValidator(m_identityValidator);
		}

		ContactAddDialog::~ContactAddDialog() {
			// Ownership is with ui->edtIdentity
			m_identityValidator = nullptr;
		}

		QString ContactAddDialog::getIdentity() const {
			return m_ui->edtIdentity->text().toUpper();
		}

		QString ContactAddDialog::getNickname() const {
			return m_ui->edtNickname->text();
		}

	}
}
