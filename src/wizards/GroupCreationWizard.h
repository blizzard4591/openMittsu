#ifndef OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_
#define OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_

#include <QSet>
#include <QWizard>

#include <memory>

#include "src/database/ContactData.h"
#include "src/protocol/ContactId.h"

#include "src/wizards/GroupCreationWizardPageInfo.h"
#include "src/wizards/GroupCreationWizardPageDone.h"

namespace Ui {
	class GroupCreationWizard;
}

namespace openmittsu {
	namespace wizards {
		class GroupCreationWizard : public QWizard {
			Q_OBJECT
		public:
			explicit GroupCreationWizard(QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent = nullptr);
			virtual ~GroupCreationWizard();
		public slots:
			void pageNextOnClick(int pageId);
		private:
			std::unique_ptr<Ui::GroupCreationWizard> m_ui;
			GroupCreationWizardPageInfo* m_groupCreationWizardPageInfo;
			GroupCreationWizardPageDone* m_groupCreationWizardPageDone;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_
