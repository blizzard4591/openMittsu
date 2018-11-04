#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_

#include <QWizard>

#include <memory>

#include "src/wizards/ContactEditWizardPageSelectContact.h"
#include "src/wizards/ContactEditWizardPageDone.h"

#include "src/dataproviders/ContactDataProvider.h"

namespace Ui {
class ContactEditWizard;
}

namespace openmittsu {
	namespace wizards {

		class ContactEditWizard : public QWizard {
			Q_OBJECT
		public:
			explicit ContactEditWizard(std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const& contactDataProvider, QWidget* parent = nullptr);
			virtual ~ContactEditWizard();
		public slots:
			void pageNextOnClick(int pageId);
		private:
			Ui::ContactEditWizard* m_ui;
			std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const m_contactDataProvider;
			

			ContactEditWizardPageSelectContact* m_contactEditWizardPageInfo;
			ContactEditWizardPageDone* m_contactEditWizardPageDone;

			bool m_haveFinished;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARD_H_
