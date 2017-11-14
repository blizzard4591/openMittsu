#ifndef OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_
#define OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_

#include <QSet>
#include <QWizard>

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
			explicit GroupCreationWizard(QHash<openmittsu::protocol::ContactId, QString> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent = nullptr);
			virtual ~GroupCreationWizard();
		public slots:
			void pageNextOnClick(int pageId);
		private:
			Ui::GroupCreationWizard *ui;
			GroupCreationWizardPageInfo* groupCreationWizardPageInfo;
			GroupCreationWizardPageDone* groupCreationWizardPageDone;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_GROUPCREATIONWIZARD_H_
