#ifndef UPDATERDIALOG_H
#define UPDATERDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class UpdaterDialog;
}

class UpdaterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdaterDialog(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString const& gitHash, QString const& channel, QString const& link, QWidget *parent = nullptr);
    ~UpdaterDialog();

public slots:
	void btnLinkOnClick();
private:
    Ui::UpdaterDialog *ui;
	QString const link;
};

#endif // UPDATERDIALOG_H
