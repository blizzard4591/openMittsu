#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

#include <QHash>
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

#include "utility/OptionMaster.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private:
    Ui::OptionsDialog *ui;

	struct OptionWidget {
		OptionMaster::OptionTypes type;
		union {
			QCheckBox* cboxPtr;
			QLineEdit* edtPtr;
		};
	};

	QHash<OptionMaster::Options, OptionWidget> optionToWidgetMap;
};

#endif // OPTIONSDIALOG_H
