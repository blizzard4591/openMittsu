#include "src/widgets/chat/ContactFileChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QPixmap>

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/StringList.h"
#include "src/widgets/ClickAwareLabel.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactFileChatWidgetItem::ContactFileChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblImage(std::make_unique<openmittsu::widgets::GifPlayer>()), m_lblCaption(std::make_unique<QLabel>()), m_mimeType(), m_fileName() {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::FILE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption.get(), 13);
			this->addWidget(m_lblImage.get());
			this->addWidget(m_lblCaption.get());

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactFileChatWidgetItem::~ContactFileChatWidgetItem() {
			//
		}

		ContactFileChatWidgetItem::LabelType ContactFileChatWidgetItem::extractData(QString const& text, QString& mimeType, QString& fileName, QString& fileSize) {
			QStringList const fields = utility::StringList::split(text);
			if (fields.size() < 8) {
				throw openmittsu::exceptions::InternalErrorException() << "Message control data could not be parsed to obtain MIME type of file: '" << text.toStdString() << "'.";
			}

			mimeType = fields.at(2);
			fileSize = fields.at(2);
			fileName = fields.at(4);
			if (mimeType.compare("image/gif", Qt::CaseInsensitive) == 0) {
				return LabelType::GIF;
			} else if (mimeType.compare("image/jpeg", Qt::CaseInsensitive) == 0 || mimeType.compare("image/png", Qt::CaseInsensitive) == 0) {
				return LabelType::IMAGE;
			} else {
				return LabelType::FILE;
			}
		}

		void ContactFileChatWidgetItem::onMessageDataChanged() {
			// [null,null,"image/gif",12345,"bla.gif",1,true,"My Caption"]
			QString const messageData = m_contactMessage.getContentAsText();

			QString m_fileSize;
			LabelType const labelType = extractData(messageData, m_mimeType, m_fileName, m_fileSize);
			LOGGER_DEBUG("FileMessage has MIME type '{}'", m_mimeType.toStdString());

			if (labelType == LabelType::FILE) {
				openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
				openmittsu::database::MediaFileItem const thumbnail = m_contactMessage.getSecondaryContentAsMediaFile();
				m_lblImage->setGifMode(true);
				if (image.isAvailable() && thumbnail.isAvailable()) {
					m_lblImage->updateData(image.getData(), thumbnail.getData());
					m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else if (labelType == LabelType::IMAGE) {
				openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
				m_lblImage->setGifMode(false);
				if (image.isAvailable()) {
					QPixmap pixmap;
					pixmap.loadFromData(image.getData());
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else {
				openmittsu::database::MediaFileItem const image = m_contactMessage.getContentAsMediaFile();
				m_lblImage->setGifMode(false);
				if (image.isAvailable()) {
					m_lblImage->setText(QString("File: '%1'\nSize: %2 Bytes").arg(m_fileName).arg(m_fileSize));
					m_lblCaption->setText(preprocessLinks(m_contactMessage.getCaption()));
				} else {
					QPixmap pixmap = image.getPixmapWithErrorMessage(500, 500);
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
