#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_

#include <QWizardPage>

namespace Ui {
class ContactEditWizardPageDone;
}

namespace openmittsu {
	namespace wizards {

		class ContactEditWizardPageDone : public QWizardPage {
			Q_OBJECT
		public:
			explicit ContactEditWizardPageDone(QWidget* parent = nullptr);
			virtual ~ContactEditWizardPageDone();

			virtual void initializePage();
		private:
			Ui::ContactEditWizardPageDone* m_ui;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGEDONE_H_
