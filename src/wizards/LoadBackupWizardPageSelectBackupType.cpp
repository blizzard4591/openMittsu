#include "src/wizards/LoadBackupWizardPageSelectBackupType.h"
#include "ui_LoadBackupWizardPageSelectBackupType.h"

#include "src/exceptions/IllegalArgumentException.h"

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/wizards/LoadBackupWizard.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageSelectBackupType::LoadBackupWizardPageSelectBackupType(QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageSelectBackupType>()) {
			m_ui->setupUi(this);
			OPENMITTSU_CONNECT(m_ui->buttonGroup, buttonToggled(int, bool), this, radioButtonsToggled());
		}

		LoadBackupWizardPageSelectBackupType::~LoadBackupWizardPageSelectBackupType() {
			//
		}

		void LoadBackupWizardPageSelectBackupType::radioButtonsToggled() {
			emit completeChanged();
		}

		bool LoadBackupWizardPageSelectBackupType::isComplete() const {
			return (getSelectedBackupType() != SelectedBackupType::Invalid);
		}

		int LoadBackupWizardPageSelectBackupType::nextId() const {
			if (m_ui->rbtnDataBackup->isChecked()) {
				return LoadBackupWizard::Pages::PAGE_DATABACKUP_DATA;
			} else if (m_ui->rbtnIdBackup->isChecked()) {
				return LoadBackupWizard::Pages::PAGE_IDBACKUP_DATA;
			} else {
				return LoadBackupWizard::Pages::PAGE_LEGACYCLIENTCONFIGURATION;
			}
		}

		LoadBackupWizardPageSelectBackupType::SelectedBackupType LoadBackupWizardPageSelectBackupType::getSelectedBackupType() const {
			if (m_ui->rbtnDataBackup->isChecked()) {
				return SelectedBackupType::DataBackup;
			} else if (m_ui->rbtnIdBackup->isChecked()) {
				return SelectedBackupType::IdBackup;
			} else if (m_ui->rbtnLegacyClientConfiguration->isChecked()) {
				return SelectedBackupType::LegacyClientConfiguration;
			} else {
				return SelectedBackupType::Invalid;
			}
		}
	}
}
