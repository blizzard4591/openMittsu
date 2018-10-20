#include "src/widgets/chat/GroupAudioChatWidgetItem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMimeData>

#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupAudioChatWidgetItem::GroupAudioChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::AUDIO) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			m_player = new Player(false, this);
			this->addWidget(m_player);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupAudioChatWidgetItem::~GroupAudioChatWidgetItem() {
			delete m_lblCaption;
		}

		void GroupAudioChatWidgetItem::onMessageDataChanged() {
			openmittsu::database::MediaFileItem const audio = m_groupMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				m_player->play(audio.getData());
			} else {
				LOGGER()->error("Failed to load audio clip for ContactAudioMessage!");
				m_lblCaption->setText("");
			}

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupAudioChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const audio = m_groupMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				QMimeData* mimeData = new QMimeData();
				mimeData->setData(QStringLiteral("audio/mp4"), audio.getData());
				clipboard->setMimeData(mimeData);
			} else {
				clipboard->clear();
			}
		}

	}
}
