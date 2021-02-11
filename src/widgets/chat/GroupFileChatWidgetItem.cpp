#include "src/widgets/chat/GroupFileChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QPixmap>

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/StringList.h"
#include "src/widgets/ImageViewer.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupFileChatWidgetItem::GroupFileChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupMediaChatWidgetItem(message, parent), m_lblImage(std::make_unique<openmittsu::widgets::GifPlayer>()), m_lblCaption(std::make_unique<QLabel>()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::FILE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption.get(), 13);
			this->addWidget(m_lblImage.get());
			this->addWidget(m_lblCaption.get());

			OPENMITTSU_CONNECT(m_lblImage.get(), clicked(), this, onImageHasBeenClicked());

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupFileChatWidgetItem::~GroupFileChatWidgetItem() {
			//
		}

		void GroupFileChatWidgetItem::onImageHasBeenClicked() {
			ImageViewer* imageViewer = new ImageViewer(m_lblImage->pixmap(Qt::ReturnByValue).toImage());
			imageViewer->show();
		}

		void GroupFileChatWidgetItem::onMessageDataChanged() {
			// [null,null,"image/gif",12345,"bla.gif",1,true,"My Caption"]
			QString const messageData = m_groupMessage.getContentAsText();

			QString m_fileSize;
			LabelType const labelType = extractDataFromJson(messageData, m_mimeType, m_fileName, m_fileSize);
			LOGGER_DEBUG("FileMessage has MIME type '{}'", m_mimeType.toStdString());

			if (labelType == LabelType::FILE) {
				openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
				openmittsu::database::MediaFileItem const thumbnail = m_groupMessage.getSecondaryContentAsMediaFile();
				m_lblImage->setGifMode(true);
				if (image.isAvailable() && thumbnail.isAvailable()) {
					m_lblImage->updateData(image.getData(), thumbnail.getData());
					m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else if (labelType == LabelType::IMAGE) {
				openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
				m_lblImage->setGifMode(false);
				if (image.isAvailable()) {
					QPixmap pixmap;
					pixmap.loadFromData(image.getData());
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else {
				openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
				m_lblImage->setGifMode(false);
				if (image.isAvailable()) {
					m_lblImage->setText(QString("File: '%1'\nSize: %2 Bytes").arg(m_fileName).arg(m_fileSize));
					m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			}

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupFileChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				QPixmap pixmap;
				pixmap.loadFromData(image.getData());
				clipboard->setPixmap(pixmap);
			} else {
				clipboard->setPixmap(image.getPixmapWithErrorMessage(500, 500));
			}
		}

		QString GroupFileChatWidgetItem::getFileExtension() const {
			if (!m_fileName.isEmpty()) {
				int pos = m_fileName.lastIndexOf('.');
				if (pos != -1) {
					return m_fileName.mid(pos + 1);
				}
			}

			return QStringLiteral("gif");
		}

		QString GroupFileChatWidgetItem::getDefaultFilename() const {
			return m_fileName;
		}

		bool GroupFileChatWidgetItem::saveMediaToFile(QString const& filename) const {
			openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
			if (image.isAvailable()) {
				return MediaChatWidgetItem::saveMediaToFile(filename, image.getData());
			} else {
				return false;
			}
		}

	}
}
