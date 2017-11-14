#include "src/widgets/chat/ContactTextChatWidgetItem.h"
#include "ui_chatwidgetitem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactTextChatWidgetItem::ContactTextChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactChatWidgetItem(message, parent), m_lblText(new QLabel()) {
			if (message.getMessageType() != openmittsu::dataproviders::messages::ContactMessageType::TEXT && !message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			this->addWidget(m_lblText);
			ChatWidgetItem::configureLabel(m_lblText, 13);

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactTextChatWidgetItem::~ContactTextChatWidgetItem() {
			delete m_lblText;
		}

		void ContactTextChatWidgetItem::onMessageDataChanged() {
			m_lblText->setText(preprocessLinks(m_contactMessage.getContentAsText()));

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactTextChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(m_contactMessage.getContentAsText());
		}

	}
}
