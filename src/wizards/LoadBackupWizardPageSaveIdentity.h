#ifndef LOADBACKUPWIZARDPAGESAVEIDENTITY_H
#define LOADBACKUPWIZARDPAGESAVEIDENTITY_H

#include <QWizardPage>
#include <QRegExpValidator>

namespace Ui {
class LoadBackupWizardPageSaveIdentity;
}

class LoadBackupWizardPageSaveIdentity : public QWizardPage
{
    Q_OBJECT

public:
    explicit LoadBackupWizardPageSaveIdentity(QWidget *parent = 0);
    ~LoadBackupWizardPageSaveIdentity();

	virtual void initializePage() override;
public slots:
	void btnPickSaveOnClick();
private:
    Ui::LoadBackupWizardPageSaveIdentity *ui;
	QRegExpValidator* fileNameValidator;
};

#endif // LOADBACKUPWIZARDPAGESAVEIDENTITY_H
