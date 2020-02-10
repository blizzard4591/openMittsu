#include "src/widgets/chat/ContactFileChatWidgetItem.h"

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

		ContactFileChatWidgetItem::ContactFileChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblImage(new openmittsu::widgets::ClickAwareLabel()), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::FILE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption, 13);
			this->addWidget(m_lblImage);
			this->addWidget(m_lblCaption);

			OPENMITTSU_CONNECT(m_lblImage, clicked(), this, onImageHasBeenClicked());

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactFileChatWidgetItem::~ContactFileChatWidgetItem() {
			delete m_lblImage;
			delete m_lblCaption;
		}

		void ContactFileChatWidgetItem::onImageHasBeenClicked() {
			//ImageViewer* imageViewer = new ImageViewer(m_lblImage->pixmap()->toImage());
			//imageViewer->show();
			if (m_mainMovie.isValid()) {
				m_mainMovie.start();
			}
		}

		void ContactFileChatWidgetItem::onMessageDataChanged() {
			QPixmap pixmap;
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				m_mainData = image.getData();
				//m_thumbnailData = m_contactMessage.get // TODO
				m_mainDataBuffer.setData(m_mainData);
				m_mainDataBuffer.open(QBuffer::ReadOnly);

				m_mainMovie.setDevice(&m_mainDataBuffer);
				m_lblImage->setMovie(&m_mainMovie);
				m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
			} else {
				pixmap = image.getPixmapWithErrorMessage(500, 500);
				m_lblImage->setPixmap(pixmap);
				m_lblCaption->setText("");
			}

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactFileChatWidgetItem::copyToClipboard() {
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

		QString ContactFileChatWidgetItem::getFileExtension() const {
			return QStringLiteral("jpg");
		}

		bool ContactFileChatWidgetItem::saveMediaToFile(QString const& filename) const {
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				return MediaChatWidgetItem::saveMediaToFile(filename, image.getData());
			} else {
				return false;
			}
		}

	}
}
