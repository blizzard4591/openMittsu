#include "src/widgets/chat/ContactVideoChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMimeData>

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactVideoChatWidgetItem::ContactVideoChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::VIDEO) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			m_player = new Player(true, this);
			this->addWidget(m_player);

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactVideoChatWidgetItem::~ContactVideoChatWidgetItem() {
			delete m_lblCaption;
		}

		void ContactVideoChatWidgetItem::setBackgroundColorAndPadding(QString const& cssColor, int padding) {
			ContactChatWidgetItem::setBackgroundColorAndPadding(cssColor, padding);

			m_player->setStyleSheet(QStringLiteral("background-color:%1;padding:%2px;").arg(cssColor).arg(padding));
		}

		void ContactVideoChatWidgetItem::onMessageDataChanged() {
			openmittsu::database::MediaFileItem const audio = m_contactMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				m_player->play(audio.getData());
			} else {
				LOGGER()->error("Failed to load audio clip for ContactAudioMessage!");
				m_lblCaption->setText("");
			}

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactVideoChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const video = m_contactMessage.getContentAsMediaFile();
			if (video.isAvailable()) {
				QMimeData* mimeData = new QMimeData();
				mimeData->setData(QStringLiteral("video/mp4"), video.getData());
				clipboard->setMimeData(mimeData);
			} else {
				clipboard->clear();
			}
		}

		QString ContactVideoChatWidgetItem::getFileExtension() const {
			return QStringLiteral("mp4");
		}

		bool ContactVideoChatWidgetItem::saveMediaToFile(QString const& filename) const {
			openmittsu::database::MediaFileItem const video = m_contactMessage.getContentAsMediaFile();
			if (video.isAvailable()) {
				return MediaChatWidgetItem::saveMediaToFile(filename, video.getData());
			} else {
				return false;
			}
		}

	}
}
