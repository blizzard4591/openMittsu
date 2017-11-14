#include "src/wizards/BackupCreationWizardPagePassword.h"
#include "ui_backupcreationwizardpagepassword.h"

#include <QRegExp>

namespace openmittsu {
	namespace wizards {

		BackupCreationWizardPagePassword::BackupCreationWizardPagePassword(QWidget *parent) : QWizardPage(parent), m_ui(new Ui::BackupCreationWizardPagePassword) {
			m_ui->setupUi(this);

			// At least one character
			m_passwordValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), m_ui->edtPassword);
			m_ui->edtPassword->setValidator(m_passwordValidator);
			m_ui->edtPassword->setEchoMode(QLineEdit::EchoMode::Password);

			registerField("edtPassword*", m_ui->edtPassword);
		}

		BackupCreationWizardPagePassword::~BackupCreationWizardPagePassword() {
			delete m_ui;
			// Ownership is with ui->edtPassword
			m_passwordValidator = nullptr;
		}

	}
}
