#include "src/wizards/LoadBackupWizardPageIdBackupData.h"
#include "ui_LoadBackupWizardPageIdBackupData.h"

#include <QRegularExpression>

#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/MakeUnique.h"
#include "src/wizards/LoadBackupWizard.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageIdBackupData::LoadBackupWizardPageIdBackupData(QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageIdBackupData>()) {
			m_ui->setupUi(this);

			// Suitable format
			m_backupStringValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression("^[a-zA-Z2-7]{4}(?:-[a-zA-Z2-7]{4})*$"));
			m_ui->edtIdBackupBackupString->setValidator(m_backupStringValidator.get());

			// At least one character
			m_passwordValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression(".+"));
			m_ui->edtIdBackupPassword->setValidator(m_passwordValidator.get());
			m_ui->edtIdBackupPassword->setEchoMode(QLineEdit::EchoMode::Password);

			registerField("edtIdBackupPassword*", m_ui->edtIdBackupPassword);
			registerField("edtIdBackupBackupString*", m_ui->edtIdBackupBackupString);
		}

		LoadBackupWizardPageIdBackupData::~LoadBackupWizardPageIdBackupData() {
			//
		}

		int LoadBackupWizardPageIdBackupData::nextId() const {
			return LoadBackupWizard::Pages::PAGE_SELECTTARGET;
		}

	}
}
