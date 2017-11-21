#include "src/widgets/chat/GroupImageChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/widgets/ImageViewer.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupImageChatWidgetItem::GroupImageChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblImage(new openmittsu::widgets::ClickAwareLabel()), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::IMAGE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption, 13);
			this->addWidget(m_lblImage);
			this->addWidget(m_lblCaption);

			OPENMITTSU_CONNECT(m_lblImage, clicked(), this, onImageHasBeenClicked());

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupImageChatWidgetItem::~GroupImageChatWidgetItem() {
			delete m_lblImage;
			delete m_lblCaption;
		}

		void GroupImageChatWidgetItem::onImageHasBeenClicked() {
			ImageViewer* imageViewer = new ImageViewer(m_lblImage->pixmap()->toImage());
			imageViewer->show();
		}

		void GroupImageChatWidgetItem::onMessageDataChanged() {
			QPixmap pixmap;
			openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsImage();
			if (image.isAvailable()) {
				pixmap.loadFromData(image.getData());
				m_lblImage->setPixmap(pixmap);
				m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
			} else {
				pixmap = image.getPixmapWithErrorMessage(500, 500);
				m_lblImage->setPixmap(pixmap);
				m_lblCaption->setText("");
			}

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupImageChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsImage();
			if (image.isAvailable()) {
				QPixmap pixmap;
				pixmap.loadFromData(image.getData());
				clipboard->setPixmap(pixmap);
			} else {
				clipboard->setPixmap(image.getPixmapWithErrorMessage(500, 500));
			}
		}

	}
}
