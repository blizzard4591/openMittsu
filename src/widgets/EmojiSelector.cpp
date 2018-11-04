#include "src/widgets/EmojiSelector.h"
#include "ui_EmojiSelector.h"

#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		EmojiSelector::EmojiSelector(QWidget* parent) : QWidget(parent), m_ui(new Ui::EmojiSelector) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(m_ui->labelActivity, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelCelebration, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelFoodAndDrink, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelNature, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelObjects, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelPeople, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelSquareButtons, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelSymbols, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(m_ui->labelTravelAndPlaces, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
		}

		EmojiSelector::~EmojiSelector() {
			delete m_ui;
		}

		void EmojiSelector::labelTextDoubleClicked(QString const& selectedText) {
			emit emojiDoubleClicked(selectedText);
		}

	}
}
