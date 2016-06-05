#ifndef OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_
#define OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_

#include <QDialog>
#include <QString>

namespace Ui {
class ContactEditDialog;
}

class ContactEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit ContactEditDialog(QString const& identity, QString const& pubKey, QString const& nickname, QWidget *parent = nullptr);
    ~ContactEditDialog();

	QString getNickname() const;
private:
    Ui::ContactEditDialog *ui;
};

#endif // OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_
