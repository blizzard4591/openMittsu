#ifndef OPENMITTSU_DIALOGS_LICENSEDIALOG_H_
#define OPENMITTSU_DIALOGS_LICENSEDIALOG_H_

#include <QDialog>

namespace Ui {
	class LicenseDialog;
}

namespace openmittsu {
	namespace dialogs {

		class LicenseDialog : public QDialog {
			Q_OBJECT
		public:
			explicit LicenseDialog(QWidget* parent = nullptr);
			virtual ~LicenseDialog();

		private:
			Ui::LicenseDialog* ui;
		};

	}
}

#endif // OPENMITTSU_DIALOGS_LICENSEDIALOG_H_
