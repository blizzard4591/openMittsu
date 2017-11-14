#ifndef OPENMITTSU_LOADBACKUPWIZARDPAGEDATABACKUPDATA_H_
#define OPENMITTSU_LOADBACKUPWIZARDPAGEDATABACKUPDATA_H_

#include <QDir>
#include <QRegularExpressionValidator>
#include <QWizardPage>

#include <memory>

namespace Ui {
class LoadBackupWizardPageDataBackupData;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizardPageDataBackupData : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageDataBackupData(QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageDataBackupData();

			virtual int nextId() const override;

			static bool isValidBackupLocation(QDir const& location, bool showWarnings = false, QWidget* parent = nullptr);
		private slots:
			void onBtnOpenBackupFolderClick();
		private:
			std::unique_ptr<Ui::LoadBackupWizardPageDataBackupData> m_ui;
			std::unique_ptr<QRegularExpressionValidator> m_passwordValidator;
			std::unique_ptr<QRegularExpressionValidator> m_backupPathValidator;
		};

	}
}

#endif // OPENMITTSU_LOADBACKUPWIZARDPAGEDATABACKUPDATA_H_
