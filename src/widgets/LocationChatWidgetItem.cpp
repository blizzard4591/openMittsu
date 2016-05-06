#include "widgets/LocationChatWidgetItem.h"
#include "ui_locationchatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>

LocationChatWidgetItem::LocationChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, double latitude, double longitude, double height, QString const& description, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, parent), ui(new Ui::LocationChatWidgetItem), latitude(latitude), longitude(longitude), height(height), description(description) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	// Set Font with UTF-8 capabilities
	QFont messagingFont(QStringLiteral("Source Sans Pro"), 12);
	messagingFont.setStyleStrategy(QFont::PreferAntialias);
	ui->lblMessageText->setFont(messagingFont);
	
	ui->lblMessageText->setOpenExternalLinks(true);
	QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(latitude).arg(longitude);
	ui->lblMessageText->setText(QString("Location: <a href=\"%1\">%1</a> - %2").arg(locationUrl).arg(description));
	ui->lblMessageText->setWordWrap(true);
	ui->lblFromTime->setWordWrap(true);

	onDataUpdated();
}

LocationChatWidgetItem::LocationChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, double latitude, double longitude, double height, QString const& description, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::LocationChatWidgetItem), latitude(latitude), longitude(longitude), height(height), description(description) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	// Set Font with UTF-8 capabilities
	QFont messagingFont(QStringLiteral("Source Sans Pro"), 12);
	messagingFont.setStyleStrategy(QFont::PreferAntialias);
	ui->lblMessageText->setFont(messagingFont);

	QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(latitude).arg(longitude);
	ui->lblMessageText->setText(QString("Location: <a href=\"%1\">%1</a> - %2").arg(locationUrl).arg(description));
	ui->lblMessageText->setWordWrap(true);
	ui->lblFromTime->setWordWrap(true);

	onDataUpdated();
}

LocationChatWidgetItem::~LocationChatWidgetItem() {
	delete ui;
}

void LocationChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(QString("Location: https://maps.google.com/?q=%1,%2 - %3").arg(latitude).arg(longitude).arg(description));
}

void LocationChatWidgetItem::setFromTimeString(QString const& text) {
	ui->lblFromTime->setText(text);
}

void LocationChatWidgetItem::setInnerAlignment(bool alignLeft) {
	if (alignLeft) {
		ui->lblMessageText->setAlignment(Qt::AlignLeft);
		ui->lblFromTime->setAlignment(Qt::AlignLeft);
	} else {
		ui->lblMessageText->setAlignment(Qt::AlignRight);
		ui->lblFromTime->setAlignment(Qt::AlignRight);
	}
}

void LocationChatWidgetItem::resizeEvent(QResizeEvent * event) {
	ChatWidgetItem::resizeEvent(event);
}

bool LocationChatWidgetItem::hasHeightForWidth() const {
	return true;
}

int LocationChatWidgetItem::heightForWidth(int w) const {
	return this->layout()->heightForWidth(w);
}