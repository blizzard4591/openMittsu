#ifndef OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_
#define OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_

#include <QDialog>
#include "ClientConfiguration.h"

namespace Ui {
class FingerprintDialog;
}

class FingerprintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FingerprintDialog(ClientConfiguration* clientConfiguration, QWidget *parent = nullptr);
    ~FingerprintDialog();

private:
    Ui::FingerprintDialog *ui;
};

#endif // OPENMITTSU_DIALOGS_FINGERPRINTDIALOG_H_
