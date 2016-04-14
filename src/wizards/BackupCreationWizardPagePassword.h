#ifndef OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_
#define OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_

#include <QWizardPage>
#include <QRegExpValidator>

namespace Ui {
class BackupCreationWizardPagePassword;
}

class BackupCreationWizardPagePassword : public QWizardPage
{
    Q_OBJECT

public:
    explicit BackupCreationWizardPagePassword(QWidget *parent = 0);
    ~BackupCreationWizardPagePassword();

	//bool isComplete() const override;
private:
    Ui::BackupCreationWizardPagePassword *ui;
	QRegExpValidator* passwordValidator;
};

#endif // OPENMITTSU_WIZARDS_BACKUPCREATIONWIZARDPAGEPASSWORD_H_
