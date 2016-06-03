#include "ChatWidget.h"
#include "ui_chatwidget.h"

#include "ContactRegistry.h"
#include <QString>
#include <QSpacerItem>
#include <QScrollBar>
#include <QPalette>
#include <QResizeEvent>
#include <QTimer>

#include "TextChatWidgetItem.h"
#include "ImageChatWidgetItem.h"

ChatWidget::ChatWidget(QWidget *parent) : QScrollArea(parent), ui(new Ui::ChatWidget) {
    ui->setupUi(this);

	this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	QSizePolicy selfSizePolicy = this->sizePolicy();
	selfSizePolicy.setHeightForWidth(true);
	this->setSizePolicy(selfSizePolicy);
	this->setWidgetResizable(true);
	
	QSizePolicy contentsSizePolicy = ui->scrollAreaWidgetContents->sizePolicy();
	contentsSizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	contentsSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
	contentsSizePolicy.setHeightForWidth(true);
	ui->scrollAreaWidgetContents->setSizePolicy(contentsSizePolicy);
	ui->scrollAreaWidgetContents->setMinimumWidth(400);

	this->setWidget(ui->scrollAreaWidgetContents);

	topLayout = new QVBoxLayout();
	ui->scrollAreaWidgetContents->setLayout(topLayout);

	QPalette myPalette(palette());
	myPalette.setColor(QPalette::Background, Qt::white);
	this->setAutoFillBackground(true);
	this->setPalette(myPalette);
}

ChatWidget::~ChatWidget() {
    delete ui;
}

void ChatWidget::scrollToBottom() {
	this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
}

void ChatWidget::resizeEvent(QResizeEvent* event) {
	// Call the super implementation
	QScrollArea::resizeEvent(event);

	/*const int newInnerWidth = ((int)(0.8 * ((double)this->width())));
	//qDebug() << "resizeEvent: innerWidth = " << newInnerWidth;
	for (int i = 0; i < items.size(); ++i) {
		items.at(i)->setWidthHint(newInnerWidth);
	}*/
	if (event->oldSize().width() != event->size().width()) {
		informAllOfSize();
	}
	this->updateGeometry();
}

void ChatWidget::informAllOfSize() {
	int newWidth = ui->scrollAreaWidgetContents->width() - 110;
	if (newWidth < 300) {
		newWidth = 300;
	}
	int totalHeight = (6 * items.size());

	// Width changed, so recalculate all children
	for (int i = 0; i < items.size(); ++i) {
		items.at(i)->setOptimalSizeHint(newWidth);
		totalHeight += items.at(i)->heightForWidth(newWidth);
	}
	//contents->setMinimumHeight(totalHeight);
	ui->scrollAreaWidgetContents->setMinimumHeight(totalHeight);
}

void ChatWidget::addItem(ChatWidgetItem* item) {
	QHBoxLayout* hboxLayout = new QHBoxLayout();
	QDateTime compareTime = QDateTime::currentDateTime();

	if (ContactRegistry::getInstance()->getSelfContact() == item->getContact()) {
		//QSpacerItem* spacerItem = new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
		//hboxLayout->addItem(spacerItem);
		hboxLayout->addWidget(item, 0, Qt::AlignRight);
		item->setInnerAlignment(false);
		item->setStyleSheet("background-color:#f7ffe8;padding:5px;");

		if (item->getIsMessageTimeSendSet()) {
			compareTime = item->getMessageTimeSend();
		}
	} else {
		//QSpacerItem* spacerItem = new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
		hboxLayout->addWidget(item, 0, Qt::AlignLeft);
		//hboxLayout->addItem(spacerItem);
		item->setInnerAlignment(true);
		item->setStyleSheet("background-color:white;padding:5px;");

		compareTime = item->getMessageTimeReceived();
	}

	
	
	int index = 0;
	for (int i = items.size() - 1; i >= 0; --i) {
		if (items.at(i)->getMessageTimeSend() <= compareTime) {
			index = i + 1;
			break;
		}
	}

	topLayout->insertLayout(index, hboxLayout);
	items.insert(index, item);

	this->update();
	informAllOfSize();
	this->updateGeometry();

	// Calling the meta object invoke() does not work here as the geometry has not yet been updated.
	//QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
	QTimer::singleShot(50, Qt::TimerType::CoarseTimer, this, SLOT(scrollToBottom()));
}

void ChatWidget::onContactDataChanged() {
	// Issue signal to all items
	for (int i = 0; i < items.size(); ++i) {
		items.at(i)->onDataUpdated();
	}
}