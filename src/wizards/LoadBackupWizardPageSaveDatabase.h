#ifndef OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASE_H_
#define OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASE_H_

#include <QWizardPage>
#include <QRegularExpressionValidator>

#include <memory>

namespace Ui {
class LoadBackupWizardPageSaveDatabase;
}

namespace openmittsu {
	namespace wizards {
		class LoadBackupWizard;

		class LoadBackupWizardPageSaveDatabase : public QWizardPage {
			Q_OBJECT
		public:
			explicit LoadBackupWizardPageSaveDatabase(LoadBackupWizard const& loadBackupWizard, QWidget* parent = nullptr);
			virtual ~LoadBackupWizardPageSaveDatabase();

			virtual void initializePage() override;
			virtual bool isComplete() const override;
			virtual int nextId() const override;
		public slots:
			void btnPickSaveOnClick();
		private:
			std::unique_ptr<Ui::LoadBackupWizardPageSaveDatabase> const m_ui;
			LoadBackupWizard const& m_loadBackupWizard;

			std::unique_ptr<QRegularExpressionValidator> m_fileNameValidator;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_LOADBACKUPWIZARDSAVEDATABASE_H_
