#ifndef OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_
#define OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_

#include <QRegularExpressionValidator>
#include <QWizard>
#include <QWizardPage>

#include <memory>

namespace Ui {
class LoadBackupWizardPageSaveDatabaseInProgress;
}

namespace openmittsu {
	namespace backup {
		class BackupReader;
	}

	namespace wizards {
		class LoadBackupWizard;

		class LoadBackupWizardPageSaveDatabaseInProgress : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageSaveDatabaseInProgress(LoadBackupWizard* parent = nullptr);
			virtual ~LoadBackupWizardPageSaveDatabaseInProgress();

			virtual void initializePage() override;
			virtual void cleanupPage() override;

			virtual bool isComplete() const override;
			virtual int nextId() const override;

			bool completedWithSuccess() const;
		public slots:
			void onProgressUpdated(int percentComplete);
			void onFinished(bool hadError, QString const& errorMessage);
		private:
			std::unique_ptr<Ui::LoadBackupWizardPageSaveDatabaseInProgress> const m_ui;
			LoadBackupWizard* const m_parent;
			bool m_isComplete;
			bool m_completedWithSuccess;
			std::unique_ptr<openmittsu::backup::BackupReader> m_backupReader;
			QString m_databaseFileName;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_
