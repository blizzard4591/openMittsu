#ifndef OPENMITTSU_WIDGETS_CHAT_DELETEMESSAGESDIALOG_H_
#define OPENMITTSU_WIDGETS_CHAT_DELETEMESSAGESDIALOG_H_

#include <QDialog>

#include <QHash>
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

#include <memory>

namespace Ui {
class DeleteMessagesDialog;
}

namespace openmittsu {
	namespace dialogs {

		class DeleteMessagesDialog : public QDialog {
			Q_OBJECT

		public:
			explicit DeleteMessagesDialog(QWidget* parent = nullptr);
			virtual ~DeleteMessagesDialog();

			enum class SelectedTab {
				TAB_BY_AGE,
				TAB_BY_DATE,
				TAB_BY_COUNT
			};
			SelectedTab getSelectedTab() const;

			// By Age
			bool getTabByAgeIsOlderThan() const;
			qint64 getTabByAgeAge() const;

			// By Date
			bool getTabByDateIsOlderThan() const;
			QDateTime getTabByDateDateTime() const;

			// By Count
			bool getTabByCountIsOldest() const;
			int getTabByCountCount() const;

		public slots:
			virtual void accept() override;
		private:
			Ui::DeleteMessagesDialog* m_ui;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_DELETEMESSAGESDIALOG_H_
