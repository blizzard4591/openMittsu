#ifndef OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEINFO_H_
#define OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEINFO_H_

#include <QWizardPage>
#include <QListWidget>
#include <QRegularExpressionValidator>

#include <QSet>

#include <memory>

#include "src/database/ContactData.h"
#include "src/dataproviders/GroupCreationProcessor.h"
#include "src/protocol/ContactId.h"

namespace Ui {
	class GroupCreationWizardPageInfo;
}

namespace openmittsu {
	namespace wizards {
		class GroupCreationWizardPageInfo : public QWizardPage {
			Q_OBJECT
		public:
			explicit GroupCreationWizardPageInfo(QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent = nullptr);
			virtual ~GroupCreationWizardPageInfo();

			bool isComplete() const override;
			virtual bool validatePage() override;
		public slots:
			void onListWidgetItemSelectionChanged();
		private:
			std::unique_ptr<Ui::GroupCreationWizardPageInfo> m_ui;
			QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> const m_knownIdentities;
			std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> const m_groupCreationProcessor;

			QRegularExpressionValidator* m_nameValidator;
			bool m_isSelectionOkay;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_GROUPCREATIONWIZARDPAGEINFO_H_
