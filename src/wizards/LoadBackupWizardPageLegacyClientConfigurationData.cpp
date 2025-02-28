#include "src/wizards/LoadBackupWizardPageLegacyClientConfigurationData.h"
#include "ui_LoadBackupWizardPageLegacyClientConfigurationData.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>

#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/LegacyClientConfigurationImporter.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/wizards/LoadBackupWizard.h"

namespace openmittsu {
	namespace wizards {

		LoadBackupWizardPageLegacyClientConfigurationData::LoadBackupWizardPageLegacyClientConfigurationData(QString const& legacyClientConfigurationFilePath, QWidget *parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::LoadBackupWizardPageLegacyClientConfigurationData>()) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(m_ui->btnOpenFile, clicked(), this, onBtnOpenFileClick());

			// At least one character
			m_backupFileValidator = std::make_unique<QRegularExpressionValidator>(QRegularExpression(".+"));
			m_ui->edtClientConfigurationLocation->setValidator(m_backupFileValidator.get());

			registerField("edtClientConfigurationLocation*", m_ui->edtClientConfigurationLocation);

			if ((!legacyClientConfigurationFilePath.isEmpty()) && isValidClientConfiguration(legacyClientConfigurationFilePath, false)) {
				m_ui->edtClientConfigurationLocation->setText(legacyClientConfigurationFilePath);
			}
		}

		LoadBackupWizardPageLegacyClientConfigurationData::~LoadBackupWizardPageLegacyClientConfigurationData() {
			//
		}

		int LoadBackupWizardPageLegacyClientConfigurationData::nextId() const {
			return LoadBackupWizard::Pages::PAGE_SELECTTARGET;
		}

		bool LoadBackupWizardPageLegacyClientConfigurationData::isValidClientConfiguration(QString const& filePath, bool showWarnings, QWidget* parent) {
			if (!QFile::exists(filePath)) {
				return false;
			}

			try {
				openmittsu::utility::LegacyClientConfigurationImporter lcci(openmittsu::utility::LegacyClientConfigurationImporter::fromFile(filePath));
			} catch (openmittsu::exceptions::BaseException& be) {
				if (showWarnings) {
					QMessageBox::warning(parent, tr("Invalid client configuration"), tr("The selected client configuration could not be parsed. Error: ").append(be.what()));
				}
				return false;
			} catch (...) {
				if (showWarnings) {
					QMessageBox::warning(parent, tr("Invalid client configuration"), tr("An unexpected error occured while parsing the file."));
				}
				return false;
			}

			return true;
		}

		void LoadBackupWizardPageLegacyClientConfigurationData::onBtnOpenFileClick() {
			QString const fileName = QFileDialog::getOpenFileName(this, tr("Select a legacy client configuration file"));
			if (fileName.isNull() || fileName.isEmpty()) {
				return;
			}

			if (!isValidClientConfiguration(fileName, true, this)) {
				m_ui->edtClientConfigurationLocation->setText("");
			} else {
				m_ui->edtClientConfigurationLocation->setText(fileName);
			}
		}

	}
}
