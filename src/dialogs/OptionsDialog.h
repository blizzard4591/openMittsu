#ifndef OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_
#define OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_

#include <QDialog>

#include <QHash>
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

#include <memory>

#include "src/options/OptionMaster.h"

namespace Ui {
	class OptionsDialog;
}

namespace openmittsu {
	namespace dialogs {

		class OptionsDialog : public QDialog {
			Q_OBJECT

		public:
			explicit OptionsDialog(std::shared_ptr<openmittsu::options::OptionMaster> const& optionMaster, QWidget* parent = nullptr);
			virtual ~OptionsDialog();
		public slots:
			virtual void accept() override;
		private:
			std::unique_ptr<Ui::OptionsDialog> m_ui;
			std::shared_ptr<openmittsu::options::OptionMaster> const m_optionMaster;

			struct OptionWidget {
				openmittsu::options::OptionTypes type;
				union {
					QCheckBox* cboxPtr;
					QLineEdit* edtPtr;
				};
			};

			QHash<openmittsu::options::Options, OptionWidget> optionToWidgetMap;
			void saveOptions();
		};

	}
}

#endif // OPENMITTSU_DIALOGS_OPTIONSDIALOG_H_
