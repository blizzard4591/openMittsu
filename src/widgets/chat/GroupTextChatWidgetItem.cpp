#include "GroupTextChatWidgetItem.h"
#include "ui_chatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupTextChatWidgetItem::GroupTextChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblText(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::GroupMessageType::TEXT && !message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblText, 13);
			this->addWidget(m_lblText);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupTextChatWidgetItem::~GroupTextChatWidgetItem() {
			delete m_lblText;
		}

		void GroupTextChatWidgetItem::onMessageDataChanged() {
			m_lblText->setText(preprocessLinks(m_groupMessage.getContentAsText()));

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupTextChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(m_groupMessage.getContentAsText());
		}

	}
}
