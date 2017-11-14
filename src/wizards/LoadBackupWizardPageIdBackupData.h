#ifndef OPENMITTSU_LOADBACKUPWIZARDPAGEIDBACKUPDATA_H_
#define OPENMITTSU_LOADBACKUPWIZARDPAGEIDBACKUPDATA_H_

#include <QWizardPage>
#include <QRegularExpressionValidator>

#include <memory>

namespace Ui {
class LoadBackupWizardPageIdBackupData;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizardPageIdBackupData : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageIdBackupData(QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageIdBackupData();

			virtual int nextId() const override;
		private:
			std::unique_ptr<Ui::LoadBackupWizardPageIdBackupData> const m_ui;
			std::unique_ptr<QRegularExpressionValidator> m_backupStringValidator;
			std::unique_ptr<QRegularExpressionValidator> m_passwordValidator;
		};

	}
}

#endif // OPENMITTSU_LOADBACKUPWIZARDPAGEIDBACKUPDATA_H_
