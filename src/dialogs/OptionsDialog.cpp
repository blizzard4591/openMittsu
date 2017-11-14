#include "src/dialogs/OptionsDialog.h"
#include "ui_OptionsDialog.h"

#include <QList>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace dialogs {

		OptionsDialog::OptionsDialog(std::shared_ptr<openmittsu::utility::OptionMaster> const& optionMaster, QWidget* parent) : QDialog(parent), m_ui(new Ui::OptionsDialog), m_optionMaster(optionMaster) {
			m_ui->setupUi(this);

			int nWidth = 300;
			int nHeight = 400;
			if (parent != nullptr) {
				setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
			} else {
				resize(nWidth, nHeight);
			}

			// Generate UI

			QList<openmittsu::utility::OptionMaster::OptionGroups> const groups = m_optionMaster->m_groupToOptionsMap.uniqueKeys();

			// Groups
			for (int i = 0, sizeKeys = groups.size(); i < sizeKeys; ++i) {
				QList<openmittsu::utility::OptionMaster::Options> const values = optionMaster->m_groupToOptionsMap.values(groups.at(i));
				if (!optionMaster->m_groupToNameMap.contains(groups.at(i))) {
					continue;
				}

				QString const groupName = optionMaster->m_groupToNameMap.value(groups.at(i));
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
					openmittsu::utility::OptionMaster::Options const option = values.at(j);
					openmittsu::utility::OptionMaster::OptionContainer const optionData = optionMaster->m_optionToOptionContainerMap.value(option);
					LOGGER_DEBUG("Option: {}", optionData.name.toStdString());

					if (optionData.type == openmittsu::utility::OptionMaster::OptionTypes::TYPE_BOOL) {
						QCheckBox* cbox = new QCheckBox();
						cbox->setChecked(optionMaster->getOptionAsBool(option));
						cbox->setText(optionData.description);

						OptionWidget ow;
						ow.type = optionData.type;
						ow.cboxPtr = cbox;

						optionToWidgetMap.insert(option, ow);
						layout->addWidget(cbox);
					} else if (optionData.type == openmittsu::utility::OptionMaster::OptionTypes::TYPE_FILEPATH) {
						QLineEdit* edt = new QLineEdit();
						edt->setText(optionMaster->getOptionAsQString(option));
						edt->setToolTip(optionData.description);

						OptionWidget ow;
						ow.type = optionData.type;
						ow.edtPtr = edt;

						optionToWidgetMap.insert(option, ow);
						layout->addWidget(edt);
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Unknown option type!";
					}
				}

				gbox->setLayout(layout);
				m_ui->verticalLayoutContainer->addWidget(gbox);
			}

			m_ui->verticalLayoutContainer->addStretch(1);
		}

		OptionsDialog::~OptionsDialog() {
			delete m_ui;
		}

		void OptionsDialog::saveOptions() {
			QHash<openmittsu::utility::OptionMaster::Options, OptionWidget>::const_iterator i = optionToWidgetMap.constBegin();
			while (i != optionToWidgetMap.constEnd()) {
				if (i.value().type == openmittsu::utility::OptionMaster::OptionTypes::TYPE_BOOL) {
					bool const value = i.value().cboxPtr->isChecked();
					m_optionMaster->setOption(i.key(), value);
				} else if (i.value().type == openmittsu::utility::OptionMaster::OptionTypes::TYPE_FILEPATH) {
					QString const value = i.value().edtPtr->text();
					m_optionMaster->setOption(i.key(), value);
				} else {
					throw openmittsu::exceptions::InternalErrorException() << "Unknown option type!";
				}
				++i;
			}
		}

		void OptionsDialog::accept() {
			LOGGER_DEBUG("OptionsDialog accepted, saving options.");
			saveOptions();

			QDialog::accept();
		}

	}
}
