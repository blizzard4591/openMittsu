#ifndef OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_
#define OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_

#include <QDialog>
#include <QString>

#include <memory>

namespace Ui {
	class ContactEditDialog;
}

namespace openmittsu {
	namespace dialogs {

		class ContactEditDialog : public QDialog {
			Q_OBJECT
		public:
			explicit ContactEditDialog(QString const& identity, QString const& pubKey, QString const& nickname, QWidget* parent = nullptr);
			virtual ~ContactEditDialog();

			QString getNickname() const;
		private:
			std::unique_ptr<Ui::ContactEditDialog> m_ui;
		};

	}
}

#endif // OPENMITTSU_DIALOGS_CONTACTEDITDIALOG_H_
