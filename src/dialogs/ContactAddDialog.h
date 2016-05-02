#ifndef CONTACTADDDIALOG_H
#define CONTACTADDDIALOG_H

#include <QDialog>
#include <QRegularExpressionValidator>

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
	QRegularExpressionValidator* identityValidator;
};

#endif // CONTACTADDDIALOG_H
