#include "src/wizards/FirstUseWizard.h"
#include "ui_FirstUseWizard.h"

#include <QButtonGroup>
#include <QRadioButton>

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {

		FirstUseWizard::FirstUseWizard(QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::FirstUseWizard>()), m_userChoice(UserChoice::CREATE_DATABASE) {
			m_ui->setupUi(this);

			m_ui->btnGroupUserChoice->setId(m_ui->rbtnLoadExisting, UserChoice::LOAD_DATABASE);
			m_ui->btnGroupUserChoice->setId(m_ui->rbtnCreateNew, UserChoice::CREATE_DATABASE);
			OPENMITTSU_CONNECT(m_ui->btnGroupUserChoice, buttonPressed(int), this, onButtonPressed(int));
		}

		FirstUseWizard::~FirstUseWizard() {
			//
		}

		FirstUseWizard::UserChoice FirstUseWizard::getUserChoice() const {
			return m_userChoice;
		}

		void FirstUseWizard::onButtonPressed(int id) {
			if (id == UserChoice::LOAD_DATABASE) {
				m_userChoice = UserChoice::LOAD_DATABASE;
			} else if (id == UserChoice::CREATE_DATABASE) {
				m_userChoice = UserChoice::CREATE_DATABASE;
			} else {
				m_userChoice = UserChoice::INVALID;
			}
		}

	}
}
