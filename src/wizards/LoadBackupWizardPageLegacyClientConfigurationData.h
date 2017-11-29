#ifndef OPENMITTSU_LOADBACKUPWIZARDPAGELEGACYCLIENTCONFIGURATIONDATA_H_
#define OPENMITTSU_LOADBACKUPWIZARDPAGELEGACYCLIENTCONFIGURATIONDATA_H_

#include <QDir>
#include <QRegularExpressionValidator>
#include <QWizardPage>

#include <memory>

namespace Ui {
class LoadBackupWizardPageLegacyClientConfigurationData;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizardPageLegacyClientConfigurationData : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageLegacyClientConfigurationData(QString const& legacyClientConfigurationFilePath, QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageLegacyClientConfigurationData();

			virtual int nextId() const override;

			static bool isValidClientConfiguration(QString const& filePath, bool showWarnings = false, QWidget* parent = nullptr);
		private slots:
			void onBtnOpenFileClick();
		private:
			QString const m_legacyClientConfigurationFilePath;
			std::unique_ptr<Ui::LoadBackupWizardPageLegacyClientConfigurationData> m_ui;
			std::unique_ptr<QRegularExpressionValidator> m_backupFileValidator;
		};

	}
}

#endif // OPENMITTSU_LOADBACKUPWIZARDPAGELEGACYCLIENTCONFIGURATIONDATA_H_
