#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_

#include <QWizardPage>
#include <QRegExpValidator>

#include <memory>

namespace Ui {
	class BackupCreationWizardPagePassword;
}

namespace openmittsu {
	namespace wizards {

		class BackupCreationWizardPagePassword : public QWizardPage {
			Q_OBJECT
		public:
			explicit BackupCreationWizardPagePassword(QWidget* parent = nullptr);
			virtual ~BackupCreationWizardPagePassword();

			//bool isComplete() const override;
		private:
			std::unique_ptr<Ui::BackupCreationWizardPagePassword> m_ui;
			QRegExpValidator* m_passwordValidator;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_
