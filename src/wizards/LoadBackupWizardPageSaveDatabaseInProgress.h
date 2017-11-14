#ifndef OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_
#define OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_

#include <QWizardPage>
#include <QRegularExpressionValidator>

#include <memory>

namespace Ui {
class LoadBackupWizardPageSaveDatabaseInProgress;
}

namespace openmittsu {
	namespace backup {
		class BackupReader;
	}

	namespace wizards {

		class LoadBackupWizardPageSaveDatabaseInProgress : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageSaveDatabaseInProgress(QWidget* parent = nullptr);
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
			bool m_isComplete;
			bool m_completedWithSuccess;
			std::unique_ptr<openmittsu::backup::BackupReader> m_backupReader;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASEINPROGRESS_H_
