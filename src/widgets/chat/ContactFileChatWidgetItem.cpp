#include "src/widgets/chat/ContactFileChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QPixmap>

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/StringList.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactFileChatWidgetItem::ContactFileChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblImage(new openmittsu::widgets::GifPlayer()), m_lblCaption(new QLabel()) {
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
		}

		void ContactFileChatWidgetItem::onMessageDataChanged() {
			QPixmap pixmap;
			openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
			openmittsu::database::MediaFileItem const thumbnail = m_contactMessage.getSecondaryContentAsMediaFile();
			
			// [null,null,"image/gif",12345,"bla.gif",1,true,"My Caption"]
			QString const messageData = m_contactMessage.getContentAsText();
			QStringList const fields = utility::StringList::split(messageData);
			if (fields.size() < 8) {
				throw openmittsu::exceptions::InternalErrorException() << "Message control data could not be parsed to obtain MIME type of file: '" << messageData.toStdString() << "'.";
			}

			m_mimeType = fields.at(2);
			m_fileName = fields.at(4);
			QString const fileSize = fields.at(3);

			LOGGER_DEBUG("FileMessage has MIME type '{}'", m_mimeType.toStdString());

			if (m_mimeType.compare("image/gif", Qt::CaseInsensitive) == 0) {
				if (image.isAvailable() && thumbnail.isAvailable()) {
					m_lblImage->updateData(image.getData(), thumbnail.getData());
					m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
				} else {
					pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else {
				m_lblImage->deactivateGifMode();
				if (image.isAvailable()) {
					m_lblImage->setText(QString("File: '%1'\nSize: %2 Bytes").arg(m_fileName).arg(fileSize));
					m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
				} else {
					pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
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
			if (!m_fileName.isEmpty()) {
				int pos = m_fileName.lastIndexOf('.');
				if (pos != -1) {
					return m_fileName.mid(pos + 1);
				}
			}

			return QStringLiteral("gif");
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
