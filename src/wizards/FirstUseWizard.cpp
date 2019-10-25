#include "src/wizards/FirstUseWizard.h"
#include "ui_FirstUseWizard.h"

#include <QButtonGroup>
#include <QRadioButton>

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace wizards {

		FirstUseWizard::FirstUseWizard(QWidget* parent) : QWizard(parent), m_ui(std::make_unique<Ui::FirstUseWizard>()), m_userChoice(UserChoice::CREATE_DATABASE) {
			m_ui->setupUi(this);

			m_ui->btnGroupUserChoice->setId(m_ui->rbtnLoadExisting, convertUserChoiceToInt(UserChoice::LOAD_DATABASE));
			m_ui->btnGroupUserChoice->setId(m_ui->rbtnCreateNew, convertUserChoiceToInt(UserChoice::CREATE_DATABASE));
			OPENMITTSU_CONNECT(m_ui->btnGroupUserChoice, buttonPressed(int), this, onButtonPressed(int));
		}

		FirstUseWizard::~FirstUseWizard() {
			//
		}

		int FirstUseWizard::convertUserChoiceToInt(UserChoice const& userChoice) {
			switch (userChoice) {
				case UserChoice::LOAD_DATABASE:
					return 1;
				case UserChoice::CREATE_DATABASE:
					return 2;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Value of UserChoice is invalid, can not convert to int: " << static_cast<int>(userChoice);
			}
		}

		FirstUseWizard::UserChoice FirstUseWizard::convertIntToUserChoice(int choiceValue) {
			switch (choiceValue) {
				case 1:
					return UserChoice::LOAD_DATABASE;
				case 2:
					return UserChoice::CREATE_DATABASE;
				default:
					return UserChoice::INVALID;
			}
		}

		FirstUseWizard::UserChoice FirstUseWizard::getUserChoice() const {
			return m_userChoice;
		}

		void FirstUseWizard::onButtonPressed(int id) {
			m_userChoice = convertIntToUserChoice(id);
		}

	}
}
