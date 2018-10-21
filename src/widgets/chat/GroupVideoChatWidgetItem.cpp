#include "src/widgets/chat/GroupVideoChatWidgetItem.h"

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

		GroupVideoChatWidgetItem::GroupVideoChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblCaption(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::VIDEO) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			m_player = new Player(true, this);
			this->addWidget(m_player);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupVideoChatWidgetItem::~GroupVideoChatWidgetItem() {
			delete m_lblCaption;
		}

		void GroupVideoChatWidgetItem::setBackgroundColorAndPadding(QString const& cssColor, int padding) {
			GroupChatWidgetItem::setBackgroundColorAndPadding(cssColor, padding);

			m_player->setStyleSheet(QStringLiteral("background-color:%1;padding:%2px;").arg(cssColor).arg(padding));
		}

		void GroupVideoChatWidgetItem::onMessageDataChanged() {
			openmittsu::database::MediaFileItem const audio = m_groupMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				m_player->play(audio.getData());
			} else {
				LOGGER()->error("Failed to load audio clip for ContactAudioMessage!");
				m_lblCaption->setText("");
			}

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupVideoChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			openmittsu::database::MediaFileItem const audio = m_groupMessage.getContentAsMediaFile();
			if (audio.isAvailable()) {
				QMimeData* mimeData = new QMimeData();
				mimeData->setData(QStringLiteral("video/mp4"), audio.getData());
				clipboard->setMimeData(mimeData);
			} else {
				clipboard->clear();
			}
		}

	}
}
