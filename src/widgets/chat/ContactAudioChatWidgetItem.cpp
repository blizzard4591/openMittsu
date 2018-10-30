#include "src/widgets/chat/ContactAudioChatWidgetItem.h"

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

		ContactAudioChatWidgetItem::ContactAudioChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactMediaChatWidgetItem(message, parent), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::AUDIO) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			m_player = new Player(false, this);
			this->addWidget(m_player);

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactAudioChatWidgetItem::~ContactAudioChatWidgetItem() {
			delete m_lblCaption;
		}

		void ContactAudioChatWidgetItem::setBackgroundColorAndPadding(QString const& cssColor, int padding) {
			ContactChatWidgetItem::setBackgroundColorAndPadding(cssColor, padding);

			m_player->setStyleSheet(QStringLiteral("background-color:%1;padding:%2px;").arg(cssColor).arg(padding));
		}

		void ContactAudioChatWidgetItem::onMessageDataChanged() {
			openmittsu::database::MediaFileItem const audio = m_contactMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				m_player->play(audio.getData());
			} else {
				LOGGER()->error("Failed to load audio clip for ContactAudioMessage!");
				m_lblCaption->setText("");
			}

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactAudioChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const audio = m_contactMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				QMimeData* mimeData = new QMimeData();
				mimeData->setData(QStringLiteral("audio/mp4"), audio.getData());
				clipboard->setMimeData(mimeData);
			} else {
				clipboard->clear();
			}
		}

		QString ContactAudioChatWidgetItem::getFileExtension() const {
			return QStringLiteral("mp4");
		}

		bool ContactAudioChatWidgetItem::saveMediaToFile(QString const& filename) const {
			openmittsu::database::MediaFileItem const audio = m_contactMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				return MediaChatWidgetItem::saveMediaToFile(filename, audio.getData());
			} else {
				return false;
			}
		}

	}
}
