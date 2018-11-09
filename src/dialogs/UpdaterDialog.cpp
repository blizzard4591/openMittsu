#include "src/dialogs/UpdaterDialog.h"
#include "ui_UpdaterDialog.h"

#include <QDesktopServices>
#include <QUrl>

#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Version.h"

namespace openmittsu {
	namespace dialogs {

		UpdaterDialog::UpdaterDialog(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString const& gitHash, QString const& channel, QString const& link, QWidget* parent)
			: QDialog(parent), m_ui(std::make_unique<Ui::UpdaterDialog>()), m_link(link) {
			m_ui->setupUi(this);

			m_ui->edtYourVersion->setText(QString::fromStdString(openmittsu::utility::Version::versionWithTagString()));

			QString versionString("%1.%2.%3");
			versionString = versionString.arg(versionMajor).arg(versionMinor).arg(versionPatch);
			if (commitsSinceTag > 0) {
				versionString.append("+").append(QString::number(commitsSinceTag));
			}
			versionString.append(" @ ").append(gitHash);
			m_ui->edtVersion->setText(versionString);
			m_ui->edtChannel->setText(channel);

			OPENMITTSU_CONNECT(m_ui->btnLink, clicked(), this, btnLinkOnClick());
		}

		UpdaterDialog::~UpdaterDialog() {
			//
		}

		void UpdaterDialog::btnLinkOnClick() {
			LOGGER_DEBUG("Opening Link in Browser.");
			QDesktopServices::openUrl(QUrl(m_link));
		}

	}
}
