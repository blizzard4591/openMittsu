#ifndef CONTACTADDDIALOG_H
#define CONTACTADDDIALOG_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
class ContactAddDialog;
}

class ContactAddDialog : public QDialog {
    Q_OBJECT

public:
    explicit ContactAddDialog(QWidget *parent = 0);
    ~ContactAddDialog();

	QString getIdentity() const;
	QString getNickname() const;
private:
    Ui::ContactAddDialog *ui;
	QRegExpValidator* identityValidator;
};

#endif // CONTACTADDDIALOG_H
