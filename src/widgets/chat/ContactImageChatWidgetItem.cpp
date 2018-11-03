#include "src/widgets/chat/ContactImageChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QPixmap>

#include "src/widgets/ClickAwareLabel.h"
#include "src/widgets/ImageViewer.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactImageChatWidgetItem::ContactImageChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblImage(new openmittsu::widgets::ClickAwareLabel()), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::IMAGE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption, 13);
			this->addWidget(m_lblImage);
			this->addWidget(m_lblCaption);

			OPENMITTSU_CONNECT(m_lblImage, clicked(), this, onImageHasBeenClicked());

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactImageChatWidgetItem::~ContactImageChatWidgetItem() {
			delete m_lblImage;
			delete m_lblCaption;
		}

		void ContactImageChatWidgetItem::onImageHasBeenClicked() {
			ImageViewer* imageViewer = new ImageViewer(m_lblImage->pixmap()->toImage());
			imageViewer->show();
		}

		void ContactImageChatWidgetItem::onMessageDataChanged() {
			QPixmap pixmap;
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				pixmap.loadFromData(image.getData());
				m_lblImage->setPixmap(pixmap);
				m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
			} else {
				pixmap = image.getPixmapWithErrorMessage(500, 500);
				m_lblImage->setPixmap(pixmap);
				m_lblCaption->setText("");
			}

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactImageChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				QPixmap pixmap;
				pixmap.loadFromData(image.getData());
				clipboard->setPixmap(pixmap);
			} else {
				clipboard->setPixmap(image.getPixmapWithErrorMessage(500, 500));
			}
		}

		QString ContactImageChatWidgetItem::getFileExtension() const {
			return QStringLiteral("jpg");
		}

		bool ContactImageChatWidgetItem::saveMediaToFile(QString const& filename) const {
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				return MediaChatWidgetItem::saveMediaToFile(filename, image.getData());
			} else {
				return false;
			}
		}

	}
}
