#include "src/widgets/chat/DeleteMessagesDialog.h"
#include "ui_DeleteMessagesDialog.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace dialogs {

		DeleteMessagesDialog::DeleteMessagesDialog(QWidget* parent) : QDialog(parent), m_ui(std::make_unique<Ui::DeleteMessagesDialog>()) {
			m_ui->setupUi(this);
		}

		DeleteMessagesDialog::~DeleteMessagesDialog() {
			//
		}

		DeleteMessagesDialog::SelectedTab DeleteMessagesDialog::getSelectedTab() const {
			auto const currentIndex = m_ui->tabWidget->currentIndex();
			if (currentIndex == 0) {
				return SelectedTab::TAB_BY_AGE;
			} else if (currentIndex == 1) {
				return SelectedTab::TAB_BY_DATE;
			} else if (currentIndex == 2) {
				return SelectedTab::TAB_BY_COUNT;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Unhandled selected tab type: " << currentIndex;
			}
		}

		// By Age
		bool DeleteMessagesDialog::getTabByAgeIsOlderThan() const {
			return m_ui->rbtnByAgeOlderThan->isChecked();
		}

		qint64 DeleteMessagesDialog::getTabByAgeAge() const {
			qint64 const number = m_ui->sboxByAgeTime->value();
			switch (m_ui->cboxByAgeTimeUnit->currentIndex()) {
				case 0: // Minutes
					return number * 60;
				case 1: // Hours
					return number * 60 * 60;
				case 2: // Days
					return number * 60 * 60 * 24;
				case 3: // Weeks
					return number * 60 * 60 * 60 * 24 * 7;
				case 4: // Months
					return number * 60 * 60 * 60 * 24 * 30;
				case 5: // Years
					return number * 60 * 60 * 60 * 24 * 365;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unhandled selected time unit type: " << m_ui->cboxByAgeTimeUnit->currentIndex();
			}
		}

		// By Date
		bool DeleteMessagesDialog::getTabByDateIsOlderThan() const {
			return m_ui->rbtnByDateOlderThan->isChecked();
		}

		QDateTime DeleteMessagesDialog::getTabByDateDateTime() const {
			return m_ui->dateTimeEdtByDate->dateTime();
		}

		// By Count
		bool DeleteMessagesDialog::getTabByCountIsOldest() const {
			return m_ui->rbtnByCountOldest->isChecked();
		}

		int DeleteMessagesDialog::getTabByCountCount() const {
			return m_ui->sboxByCount->value();
		}

		void DeleteMessagesDialog::accept() {
			QDialog::accept();
		}

	}
}
