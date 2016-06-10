#include "ImageChatWidgetItem.h"
#include "ui_imagechatwidgetitem.h"

#include "widgets/ImageViewer.h"
#include "utility/QObjectConnectionMacro.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>

ImageChatWidgetItem::ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QPixmap const& image, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, parent), ui(new Ui::ImageChatWidgetItem), image(image) {
	ui->setupUi(this);
	
	OPENMITTSU_CONNECT(ui->lblImage, clicked(), this, lblImageOnClick());

	ui->lblImage->setPixmap(image);
	onDataUpdated();
}

ImageChatWidgetItem::ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QPixmap const& image, QWidget *parent) : ChatWidgetItem(contact, senderAndMessageId, timeSend, timeReceived, parent), ui(new Ui::ImageChatWidgetItem), image(image) {
	ui->setupUi(this);

	OPENMITTSU_CONNECT(ui->lblImage, clicked(), this, lblImageOnClick());

	ui->lblImage->setPixmap(image);
	onDataUpdated();
}

ImageChatWidgetItem::~ImageChatWidgetItem() {
	delete ui;
}

void ImageChatWidgetItem::lblImageOnClick() {
	ImageViewer* imageViewer = new ImageViewer(image.toImage());
	imageViewer->show();
}

void ImageChatWidgetItem::copyToClipboard() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setPixmap(image);
}

void ImageChatWidgetItem::setFromString(QString const& fromString) {
	ui->lblFrom->setText(fromString);
}

void ImageChatWidgetItem::setTimeAndStatusString(QString const& timeAndStatusString) {
	ui->lblTimeAndStatus->setText(timeAndStatusString);
}

void ImageChatWidgetItem::setInnerAlignment(bool alignLeft) {
	if (alignLeft) {
		ui->lblFrom->setAlignment(Qt::AlignLeft);
		ui->lblImage->setAlignment(Qt::AlignLeft);
		ui->lblTimeAndStatus->setAlignment(Qt::AlignLeft);
	} else {
		ui->lblFrom->setAlignment(Qt::AlignRight);
		ui->lblImage->setAlignment(Qt::AlignRight);
		ui->lblTimeAndStatus->setAlignment(Qt::AlignRight);
	}
}