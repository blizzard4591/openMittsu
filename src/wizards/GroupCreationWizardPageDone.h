#ifndef OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEDONE_H_
#define OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEDONE_H_

#include <QWizardPage>

#include <memory>

namespace Ui {
	class GroupCreationWizardPageDone;
}

namespace openmittsu {
	namespace wizards {
		class GroupCreationWizardPageDone : public QWizardPage {
			Q_OBJECT
		public:
			explicit GroupCreationWizardPageDone(QWidget* parent = nullptr);
			virtual ~GroupCreationWizardPageDone();

		private:
			std::unique_ptr<Ui::GroupCreationWizardPageDone> m_ui;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEDONE_H_
