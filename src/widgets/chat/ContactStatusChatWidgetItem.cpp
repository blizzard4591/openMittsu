#include "src/widgets/chat/ContactStatusChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		ContactStatusChatWidgetItem::ContactStatusChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactChatWidgetItem(message, parent), m_lblStatus(new QLabel()) {
			if (!message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle non-status message with type " << openmittsu::dataproviders::messages::ContactMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblStatus, 13);
			this->addWidget(m_lblStatus);

			onContactDataChanged();
			onMessageDataChanged();
		}

		ContactStatusChatWidgetItem::~ContactStatusChatWidgetItem() {
			delete m_lblStatus;
		}

		void ContactStatusChatWidgetItem::onMessageDataChanged() {
			m_lblStatus->setText(preprocessLinks(m_contactMessage.getContentAsText()));

			ContactChatWidgetItem::onMessageDataChanged();
		}

		void ContactStatusChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(m_contactMessage.getContentAsText());
		}

	}
}
