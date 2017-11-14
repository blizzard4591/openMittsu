#ifndef OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_
#define OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_

#include <QDialog>

#include <QHash>
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

#include <memory>

#include "src/utility/OptionMaster.h"

namespace Ui {
class OptionsDialog;
}

namespace openmittsu {
	namespace dialogs {

		class OptionsDialog : public QDialog {
			Q_OBJECT

		public:
			explicit OptionsDialog(std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster, QWidget* parent = nullptr);
			virtual ~OptionsDialog();
		public slots:
			virtual void accept() override;
		private:
			Ui::OptionsDialog* m_ui;
			std::shared_ptr<openmittsu::utility::OptionMaster> const m_optionMaster;

			struct OptionWidget {
				openmittsu::utility::OptionMaster::OptionTypes type;
				union {
					QCheckBox* cboxPtr;
					QLineEdit* edtPtr;
				};
			};

			QHash<openmittsu::utility::OptionMaster::Options, OptionWidget> optionToWidgetMap;
			void saveOptions();
		};

	}
}

#endif // OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_
