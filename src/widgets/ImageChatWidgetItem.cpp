#include "ImageChatWidgetItem.h"
#include "ui_imagechatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>

ImageChatWidgetItem::ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QPixmap const& image, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, parent), ui(new Ui::ImageChatWidgetItem), image(image) {
	ui->setupUi(this);

	ui->lblImage->setPixmap(image);
	ui->lblFromTime->setWordWrap(true);
	onDataUpdated();
}

ImageChatWidgetItem::ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QPixmap const& image, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::ImageChatWidgetItem), image(image) {
	ui->setupUi(this);

	ui->lblImage->setPixmap(image);
	ui->lblFromTime->setWordWrap(true);
	onDataUpdated();
}

ImageChatWidgetItem::~ImageChatWidgetItem() {
	delete ui;
}

void ImageChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setPixmap(image);
}

void ImageChatWidgetItem::setFromTimeString(QString const& text) {
	ui->lblFromTime->setText(text);
}

void ImageChatWidgetItem::setInnerAlignment(bool alignLeft) {
	if (alignLeft) {
		ui->lblImage->setAlignment(Qt::AlignLeft);
		ui->lblFromTime->setAlignment(Qt::AlignLeft);
	} else {
		ui->lblImage->setAlignment(Qt::AlignRight);
		ui->lblFromTime->setAlignment(Qt::AlignRight);
	}
}