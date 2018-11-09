#ifndef OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_
#define OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_

#include <QWizardPage>
#include <QListWidget>
#include <QRegExpValidator>

#include <memory>

#include "src/dataproviders/ContactDataProvider.h"

namespace Ui {
	class ContactEditWizardPageSelectContact;
}

namespace openmittsu {
	namespace wizards {

		class ContactEditWizardPageSelectContact : public QWizardPage {
			Q_OBJECT
		public:
			explicit ContactEditWizardPageSelectContact(std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const& contactDataProvider, QWidget* parent = nullptr);
			virtual ~ContactEditWizardPageSelectContact();

			bool isComplete() const override;
			QListWidget const* getSelectedGroupMembersWidgetPointer() const;
		public slots:
			void onListWidgetItemSelectionChanged();
		private:
			std::unique_ptr<Ui::ContactEditWizardPageSelectContact> m_ui;
			std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const m_contactDataProvider;

			QRegExpValidator* m_nameValidator;
			bool m_isSelectionOkay;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_CONTACTEDITWIZARDPAGESELECTCONTACT_H_
