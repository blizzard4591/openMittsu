#ifndef OPENMITTSU_LOADBACKUPWIZARDPAGESELECTBACKUPTYPE_H_
#define OPENMITTSU_LOADBACKUPWIZARDPAGESELECTBACKUPTYPE_H_

#include <QWizardPage>

namespace Ui {
class LoadBackupWizardPageSelectBackupType;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizardPageSelectBackupType : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageSelectBackupType(QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageSelectBackupType();

			virtual bool isComplete() const override;
			virtual int nextId() const override;

			enum class SelectedBackupType {
				Invalid,
				IdBackup,
				DataBackup
			};
			SelectedBackupType getSelectedBackupType() const;
		private slots:
			void radioButtonsToggled();
		private:
			Ui::LoadBackupWizardPageSelectBackupType* m_ui;
		};

	}
}

#endif // OPENMITTSU_LOADBACKUPWIZARDPAGESELECTBACKUPTYPE_H_
