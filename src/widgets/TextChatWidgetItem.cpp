#include "TextChatWidgetItem.h"
#include "ui_textchatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>

TextChatWidgetItem::TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QString const& message, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, parent), ui(new Ui::TextChatWidgetItem), message(message) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	ui->lblMessageText->setText(preprocessLinks(message));

	onDataUpdated();
}

TextChatWidgetItem::TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QString const& message, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::TextChatWidgetItem), message(message) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	ui->lblMessageText->setText(preprocessLinks(message));

	onDataUpdated();
}

TextChatWidgetItem::~TextChatWidgetItem() {
	delete ui;
}

void TextChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(message);
}

void TextChatWidgetItem::setFromString(QString const& fromString) {
	ui->lblFrom->setText(fromString);
}

void TextChatWidgetItem::setTimeAndStatusString(QString const& timeAndStatusString) {
	ui->lblTimeAndStatus->setText(timeAndStatusString);
}

void TextChatWidgetItem::setInnerAlignment(bool alignLeft) {
	if (alignLeft) {
		ui->lblFrom->setAlignment(Qt::AlignLeft);
		ui->lblMessageText->setAlignment(Qt::AlignLeft);
		ui->lblTimeAndStatus->setAlignment(Qt::AlignLeft);
	} else {
		ui->lblFrom->setAlignment(Qt::AlignRight);
		ui->lblMessageText->setAlignment(Qt::AlignRight);
		ui->lblTimeAndStatus->setAlignment(Qt::AlignRight);
	}
}

void TextChatWidgetItem::resizeEvent(QResizeEvent * event) {
	ChatWidgetItem::resizeEvent(event);
}

bool TextChatWidgetItem::hasHeightForWidth() const {
	return true;
}

int TextChatWidgetItem::heightForWidth(int w) const {
	return this->layout()->heightForWidth(w);
}

