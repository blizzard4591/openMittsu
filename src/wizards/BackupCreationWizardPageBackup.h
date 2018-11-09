#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_

#include <QWizardPage>

#include <memory>

#include "src/backup/IdentityBackup.h"

namespace Ui {
	class BackupCreationWizardPageBackup;
}

namespace openmittsu {
	namespace wizards {
		class BackupCreationWizardPageBackup : public QWizardPage {
			Q_OBJECT
		public:
			explicit BackupCreationWizardPageBackup(openmittsu::backup::IdentityBackup const& identityBackup, QWidget* parent = nullptr);
			virtual ~BackupCreationWizardPageBackup();

			//bool isComplete() const override;
			virtual void initializePage() override;
		private:
			std::unique_ptr<Ui::BackupCreationWizardPageBackup> m_ui;
			openmittsu::backup::IdentityBackup const m_identityBackup;
		};
	}
}

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEBACKUP_H_
