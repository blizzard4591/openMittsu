#ifndef OPENMITTSU_DIALOGS_UPDATERDIALOG_H_
#define OPENMITTSU_DIALOGS_UPDATERDIALOG_H_

#include <QDialog>
#include <QString>

#include <memory>

namespace Ui {
	class UpdaterDialog;
}

namespace openmittsu {
	namespace dialogs {

		class UpdaterDialog : public QDialog {
			Q_OBJECT

		public:
			explicit UpdaterDialog(int versionMajor, int versionMinor, int versionPatch, int commitsSinceTag, QString const& gitHash, QString const& channel, QString const& link, QWidget* parent = nullptr);
			virtual ~UpdaterDialog();

		public slots:
			void btnLinkOnClick();
		private:
			std::unique_ptr<Ui::UpdaterDialog> m_ui;
			QString const m_link;
		};

	}
}

#endif // OPENMITTSU_DIALOGS_UPDATERDIALOG_H_
