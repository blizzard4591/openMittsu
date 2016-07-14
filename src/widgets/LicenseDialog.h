#ifndef OPENMITTSU_WIDGETS_LICENSEDIALOG_H_
#define OPENMITTSU_WIDGETS_LICENSEDIALOG_H_

#include <QDialog>

namespace Ui {
class LicenseDialog;
}

class LicenseDialog : public QDialog {
    Q_OBJECT

public:
    explicit LicenseDialog(QWidget *parent = 0);
    ~LicenseDialog();

private:
    Ui::LicenseDialog *ui;
};

#endif // OPENMITTSU_WIDGETS_LICENSEDIALOG_H_
