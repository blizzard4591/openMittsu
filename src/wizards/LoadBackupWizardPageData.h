#ifndef OPENMITTSU_LOADBACKUPWIZARDPAGEDATA_H_
#define OPENMITTSU_LOADBACKUPWIZARDPAGEDATA_H_

#include <QWizardPage>
#include <QRegExpValidator>

namespace Ui {
class LoadBackupWizardPageData;
}

class LoadBackupWizardPageData : public QWizardPage
{
    Q_OBJECT

public:
    explicit LoadBackupWizardPageData(QWidget *parent = 0);
    ~LoadBackupWizardPageData();

	//bool isComplete() const override;
private:
    Ui::LoadBackupWizardPageData *ui;
	QRegExpValidator* backupStringValidator;
	QRegExpValidator* passwordValidator;
};

#endif // OPENMITTSU_LOADBACKUPWIZARDPAGEDATA_H_
