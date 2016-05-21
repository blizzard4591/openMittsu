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
	
	ui->lblMessageText->setOpenExternalLinks(true);
	QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(latitude).arg(longitude);
	ui->lblMessageText->setText(QString(tr("Location: <a href=\"%1\">%1</a> - %2")).arg(locationUrl).arg(description));

	onDataUpdated();
}

LocationChatWidgetItem::LocationChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, double latitude, double longitude, double height, QString const& description, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::LocationChatWidgetItem), latitude(latitude), longitude(longitude), height(height), description(description) {
	ui->setupUi(this);

	QSizePolicy sp = this->sizePolicy();
	sp.setHeightForWidth(true);
	this->setSizePolicy(sp);

	QString const locationUrl = QString(QStringLiteral("https://maps.google.com/?q=%1,%2")).arg(latitude).arg(longitude);
	ui->lblMessageText->setText(QString(tr("Location: <a href=\"%1\">%1</a> - %2")).arg(locationUrl).arg(description));

	onDataUpdated();
}

LocationChatWidgetItem::~LocationChatWidgetItem() {
	delete ui;
}

void LocationChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(QString(tr("Location: https://maps.google.com/?q=%1,%2 - %3")).arg(latitude).arg(longitude).arg(description));
}

void LocationChatWidgetItem::setFromString(QString const& fromString) {
	ui->lblFrom->setText(fromString);
}

void LocationChatWidgetItem::setTimeAndStatusString(QString const& timeAndStatusString) {
	ui->lblTimeAndStatus->setText(timeAndStatusString);
}

void LocationChatWidgetItem::setInnerAlignment(bool alignLeft) {
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

void LocationChatWidgetItem::resizeEvent(QResizeEvent * event) {
	ChatWidgetItem::resizeEvent(event);
}

bool LocationChatWidgetItem::hasHeightForWidth() const {
	return true;
}

int LocationChatWidgetItem::heightForWidth(int w) const {
	return this->layout()->heightForWidth(w);
}