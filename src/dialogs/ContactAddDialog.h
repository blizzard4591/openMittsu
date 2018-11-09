#ifndef OPENMITTSU_DIALOGS_CONTACTADDDIALOG_H_
#define OPENMITTSU_DIALOGS_CONTACTADDDIALOG_H_

#include <QDialog>
#include <QRegularExpressionValidator>

#include <memory>

namespace Ui {
	class ContactAddDialog;
}

namespace openmittsu {
	namespace dialogs {

		class ContactAddDialog : public QDialog {
			Q_OBJECT
		public:
			explicit ContactAddDialog(QWidget* parent = nullptr);
			virtual ~ContactAddDialog();

			QString getIdentity() const;
			QString getNickname() const;
		private:
			std::unique_ptr<Ui::ContactAddDialog> m_ui;
			QRegularExpressionValidator* m_identityValidator;
		};

	}
}

#endif // OPENMITTSU_DIALOGS_CONTACTADDDIALOG_H_
