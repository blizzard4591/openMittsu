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
	ui->lblMessageText->setWordWrap(true);
	ui->lblFromTime->setWordWrap(true);

	onDataUpdated();
}

TextChatWidgetItem::TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QString const& message, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::TextChatWidgetItem), message(message) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	ui->lblMessageText->setText(preprocessLinks(message));
	ui->lblMessageText->setWordWrap(true);
	ui->lblFromTime->setWordWrap(true);

	onDataUpdated();
}

TextChatWidgetItem::~TextChatWidgetItem() {
	delete ui;
}

void TextChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(message);
}

void TextChatWidgetItem::setFromTimeString(QString const& text) {
	ui->lblFromTime->setText(text);
}

void TextChatWidgetItem::setInnerAlignment(bool alignLeft) {
	if (alignLeft) {
		ui->lblMessageText->setAlignment(Qt::AlignLeft);
		ui->lblFromTime->setAlignment(Qt::AlignLeft);
	} else {
		ui->lblMessageText->setAlignment(Qt::AlignRight);
		ui->lblFromTime->setAlignment(Qt::AlignRight);
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

