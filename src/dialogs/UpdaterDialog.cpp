#include "UpdaterDialog.h"
#include "ui_UpdaterDialog.h"

#include <QDesktopServices>
#include <QUrl>

#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"

UpdaterDialog::UpdaterDialog(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString const& gitHash, QString const& channel, QString const& link, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdaterDialog), link(link) {
    ui->setupUi(this);

	QString versionString("%1.%2.%3");
	versionString = versionString.arg(versionMajor).arg(versionMinor).arg(versionPatch);
	if (commitsSinceTag > 0) {
		versionString.append("+").append(QString::number(commitsSinceTag));
	}
	versionString.append(" @ ").append(gitHash);
	ui->edtVersion->setText(versionString);
	ui->edtChannel->setText(channel);

	OPENMITTSU_CONNECT(ui->btnLink, clicked(), this, btnLinkOnClick());
}

UpdaterDialog::~UpdaterDialog() {
    delete ui;
}

void UpdaterDialog::btnLinkOnClick() {
	LOGGER_DEBUG("Opening Link in Browser.");
	QDesktopServices::openUrl(QUrl(link));
}
