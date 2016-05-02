#include "TextChatWidgetItem.h"
#include "ui_textchatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QRegularExpression>

TextChatWidgetItem::TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QString const& message, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, parent), ui(new Ui::TextChatWidgetItem), message(message) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	// Set Font with UTF-8 capabilities
	QFont messagingFont(QStringLiteral("Source Sans Pro"), 12);
	messagingFont.setStyleStrategy(QFont::PreferAntialias);
	ui->lblMessageText->setFont(messagingFont);
	
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

	// Set Font with UTF-8 capabilities
	QFont messagingFont(QStringLiteral("Source Sans Pro"), 12);
	messagingFont.setStyleStrategy(QFont::PreferAntialias);
	ui->lblMessageText->setFont(messagingFont);

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

QString TextChatWidgetItem::preprocessLinks(QString const& text) {
	QRegularExpression regExp(QStringLiteral("\\b((https?|ftp|file)://[-A-Z0-9+&@#/%?=~_|$!:,.;]*[A-Z0-9+&@#/%=~_|$])"), QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);

	QString result = text;
	 return result.replace(regExp, QStringLiteral("<a href=\"\\1\">\\1</a>"));
}
