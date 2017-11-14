#ifndef OPENMITTSU_WIZARDS_LOADBACKUPWIZARDPAGEDONE_H_
#define OPENMITTSU_WIZARDS_LOADBACKUPWIZARDPAGEDONE_H_

#include <QWizardPage>

namespace Ui {
class LoadBackupWizardPageDone;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizardPageDone : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageDone(QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageDone();

		private:
			Ui::LoadBackupWizardPageDone* m_ui;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_LOADBACKUPWIZARDPAGEDONE_H_
