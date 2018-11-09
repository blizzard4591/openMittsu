#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_

#include <QWizard>

#include <memory>

#include "src/backup/IdentityBackup.h"
#include "src/wizards/BackupCreationWizardPageBackup.h"
#include "src/wizards/BackupCreationWizardPagePassword.h"

namespace Ui {
	class BackupCreationWizard;
}

namespace openmittsu {
	namespace wizards {
		class BackupCreationWizard : public QWizard {
			Q_OBJECT
		public:
			explicit BackupCreationWizard(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent = nullptr);
			virtual ~BackupCreationWizard();
		public slots:
			void pageNextOnClick(int pageId);
		private:
			std::unique_ptr<Ui::BackupCreationWizard> m_ui;

			BackupCreationWizardPagePassword* m_backupCreationWizardPagePassword;
			BackupCreationWizardPageBackup* m_backupCreationWizardPageBackup;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
