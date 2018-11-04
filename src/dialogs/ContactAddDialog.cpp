#include "src/dialogs/ContactAddDialog.h"
#include "ui_contactadddialog.h"

#include <QRegularExpression>

namespace openmittsu {
	namespace dialogs {

		ContactAddDialog::ContactAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ContactAddDialog) {
			ui->setupUi(this);

			identityValidator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[a-zA-Z0-9*][a-zA-Z0-9]{7}")), ui->edtIdentity);
			ui->edtIdentity->setValidator(identityValidator);
		}

		ContactAddDialog::~ContactAddDialog() {
			delete ui;
		}

		QString ContactAddDialog::getIdentity() const {
			return ui->edtIdentity->text().toUpper();
		}

		QString ContactAddDialog::getNickname() const {
			return ui->edtNickname->text();
		}

	}
}
