#include "EmojiSelector.h"
#include "ui_EmojiSelector.h"

EmojiSelector::EmojiSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmojiSelector)
{
    ui->setupUi(this);
}

EmojiSelector::~EmojiSelector()
{
    delete ui;
}
