#ifndef SHOWIDENTITYANDPUBLICKEYDIALOG_H
#define SHOWIDENTITYANDPUBLICKEYDIALOG_H

#include <QDialog>
#include "ClientConfiguration.h"

namespace Ui {
class ShowIdentityAndPublicKeyDialog;
}

class ShowIdentityAndPublicKeyDialog : public QDialog {
    Q_OBJECT

public:
    explicit ShowIdentityAndPublicKeyDialog(ClientConfiguration* clientConfiguration, QWidget *parent = 0);
    ~ShowIdentityAndPublicKeyDialog();

private:
    Ui::ShowIdentityAndPublicKeyDialog *ui;
};

#endif // SHOWIDENTITYANDPUBLICKEYDIALOG_H
