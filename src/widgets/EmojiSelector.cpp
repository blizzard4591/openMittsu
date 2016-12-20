#include "EmojiSelector.h"
#include "ui_EmojiSelector.h"

#include "utility/QObjectConnectionMacro.h"

EmojiSelector::EmojiSelector(QWidget *parent) : QWidget(parent), ui(new Ui::EmojiSelector) {
    ui->setupUi(this);

	OPENMITTSU_CONNECT(ui->labelActivity, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelCelebration, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelFoodAndDrink, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelNature, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelObjects, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelPeople, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelSquareButtons, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelSymbols, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
	OPENMITTSU_CONNECT(ui->labelTravelAndPlaces, doubleClickSelectedText(QString const&), this, labelTextDoubleClicked(QString const&));
}

EmojiSelector::~EmojiSelector() {
    delete ui;
}

void EmojiSelector::labelTextDoubleClicked(QString const& selectedText) {
	emit emojiDoubleClicked(selectedText);
}
