#ifndef OPENMITTSU_WIZARDS_LOADBACKUPWIZARD_H_
#define OPENMITTSU_WIZARDS_LOADBACKUPWIZARD_H_

#include <QString>
#include <QWizard>

#include "src/wizards/LoadBackupWizardPageSelectBackupType.h"
#include "src/wizards/LoadBackupWizardPageDataBackupData.h"
#include "src/wizards/LoadBackupWizardPageIdBackupData.h"
#include "src/wizards/LoadBackupWizardPageLegacyClientConfigurationData.h"
#include "src/wizards/LoadBackupWizardPageSaveDatabase.h"
#include "src/wizards/LoadBackupWizardPageSaveDatabaseInProgress.h"
#include "src/wizards/LoadBackupWizardPageDone.h"

#include <memory>

namespace Ui {
class LoadBackupWizard;
}

namespace openmittsu {
	namespace wizards {

		class LoadBackupWizard : public QWizard {
			Q_OBJECT
		public:
			enum Pages {
				PAGE_SELECTBACKUPTYPE,
				PAGE_IDBACKUP_DATA,
				PAGE_DATABACKUP_DATA,
				PAGE_LEGACYCLIENTCONFIGURATION,
				PAGE_SELECTTARGET,
				PAGE_DATABASE_SAVE_IN_PROGRESS,
				PAGE_DONE
			};

			explicit LoadBackupWizard(QString const& legacyClientConfigurationFileName, QWidget* parent = nullptr);
			virtual ~LoadBackupWizard();

			QString getDatabaseFileName() const;
			void setDatabaseFileName(QString const& fileName);

			friend class LoadBackupWizardPageSaveDatabase;
		public slots:
			void pageNextOnClick(int pageId);
		private:
			std::unique_ptr<Ui::LoadBackupWizard> const m_ui;

			std::unique_ptr<LoadBackupWizardPageSelectBackupType> m_loadBackupWizardPageSelectBackupType;
			std::unique_ptr<LoadBackupWizardPageDataBackupData> m_loadBackupWizardPageDataBackupData;
			std::unique_ptr<LoadBackupWizardPageIdBackupData> m_loadBackupWizardPageIdBackupData;
			std::unique_ptr<LoadBackupWizardPageLegacyClientConfigurationData> m_loadBackupWizardPageLegacyClientConfigurationData;
			std::unique_ptr<LoadBackupWizardPageSaveDatabase> m_loadBackupWizardPageSaveDatabase;
			std::unique_ptr<LoadBackupWizardPageSaveDatabaseInProgress> m_loadBackupWizardPageSaveDatabaseInProgress;
			std::unique_ptr<LoadBackupWizardPageDone> m_loadBackupWizardPageDone;

			bool m_hasPresetBackup;

			QString m_backupString;
			QString m_backupPassword;
			QString m_databaseFileName;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_LOADBACKUPWIZARD_H_
