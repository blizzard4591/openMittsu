#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_

#include <QWizard>

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
			Ui::BackupCreationWizard* ui;

			BackupCreationWizardPagePassword* backupCreationWizardPagePassword;
			BackupCreationWizardPageBackup* backupCreationWizardPageBackup;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARD_H_
