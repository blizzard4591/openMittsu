#include "src/widgets/chat/GroupFileChatWidgetItem.h"

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

		GroupFileChatWidgetItem::GroupFileChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupMediaChatWidgetItem(message, parent), m_lblImage(new openmittsu::widgets::GifPlayer()), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::FILE) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblCaption, 13);
			this->addWidget(m_lblImage);
			this->addWidget(m_lblCaption);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupFileChatWidgetItem::~GroupFileChatWidgetItem() {
			delete m_lblImage;
			delete m_lblCaption;
		}

		void GroupFileChatWidgetItem::onMessageDataChanged() {
			QPixmap pixmap;
			openmittsu::database::MediaFileItem const image = m_groupMessage.getContentAsMediaFile();
			openmittsu::database::MediaFileItem const thumbnail = m_groupMessage.getSecondaryContentAsMediaFile();
			
			// [null,null,"image/gif",12345,"bla.gif",1,true,"My Caption"]
			QString const messageData = m_groupMessage.getContentAsText();
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
					m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
				} else {
					pixmap = image.getPixmapWithErrorMessage(500, 500);
					m_lblImage->setPixmap(pixmap);
					m_lblCaption->setText("");
				}
			} else {
				m_lblImage->deactivateGifMode();
				if (image.isAvailable()) {
					m_lblImage->setText(QString("File: '%1'\nSize: %2 Bytes").arg(m_fileName).arg(fileSize));
					m_lblCaption->setText(preprocessLinks(m_groupMessage.getCaption()));
				} else {
					pixmap = image.getPixmapWithErrorMessage(500, 500);
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
