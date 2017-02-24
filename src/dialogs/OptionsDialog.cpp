#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

#include <QList>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>

#include "utility/Logging.h"

OptionsDialog::OptionsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::OptionsDialog) {
    ui->setupUi(this);

	int nWidth = 300;
	int nHeight = 400;
	if (parent != nullptr) {
		setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
	} else {
		resize(nWidth, nHeight);
	}

	// Generate UI
	OptionMaster* optionMaster = OptionMaster::getInstance();

	QList<OptionMaster::OptionGroups> const groups = optionMaster->groupToOptionsMap.uniqueKeys();

	// Groups
	for (int i = 0, sizeKeys = groups.size(); i < sizeKeys; ++i) {
		QList<OptionMaster::Options> const values = optionMaster->groupToOptionsMap.values(groups.at(i));
		if (!optionMaster->groupToNameMap.contains(groups.at(i))) {
			continue;
		}
		
		QString const groupName = optionMaster->groupToNameMap.value(groups.at(i));
		if (groupName.isEmpty()) {
			continue;
		}
		LOGGER_DEBUG("Group: {}", groupName.toStdString());

		int const sizeValues = values.size();
		if (sizeValues < 1) {
			continue;
		}

		QGroupBox* gbox = new QGroupBox();
		gbox->setTitle(groupName);
		QVBoxLayout* layout = new QVBoxLayout();


		for (int j = 0; j < sizeValues; ++j) {
			OptionMaster::Options const option = values.at(j);
			OptionMaster::OptionContainer const optionData = optionMaster->optionToOptionContainerMap.value(option);
			LOGGER_DEBUG("Option: {}", optionData.name.toStdString());

			if (optionData.type == OptionMaster::OptionTypes::TYPE_BOOL) {
				QCheckBox* cbox = new QCheckBox();
				cbox->setChecked(optionMaster->getOptionAsBool(option));
				cbox->setText(optionData.description);

				OptionWidget ow;
				ow.type = optionData.type;
				ow.cboxPtr = cbox;

				optionToWidgetMap.insert(option, ow);
				layout->addWidget(cbox);
			} else if (optionData.type == OptionMaster::OptionTypes::TYPE_FILEPATH) {
				QLineEdit* edt = new QLineEdit();
				edt->setText(optionMaster->getOptionAsQString(option));
				edt->setToolTip(optionData.description);

				OptionWidget ow;
				ow.type = optionData.type;
				ow.edtPtr = edt;

				optionToWidgetMap.insert(option, ow);
				layout->addWidget(edt);
			}
		}

		gbox->setLayout(layout);
		ui->verticalLayoutContainer->addWidget(gbox);
	}

	ui->verticalLayoutContainer->addStretch(1);
}

OptionsDialog::~OptionsDialog() {
    delete ui;
}
