#include "GroupStatusChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"

namespace openmittsu {
	namespace widgets {

		GroupStatusChatWidgetItem::GroupStatusChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), m_lblStatus(new QLabel()) {
			if (!message.isStatusMessage()) {
				throw openmittsu::exceptions::InternalErrorException() << "Can not handle non-status message with type " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(message.getMessageType()) << ".";
			}

			ChatWidgetItem::configureLabel(m_lblStatus, 13);
			this->addWidget(m_lblStatus);

			onContactDataChanged();
			onMessageDataChanged();
		}

		GroupStatusChatWidgetItem::~GroupStatusChatWidgetItem() {
			delete m_lblStatus;
		}

		void GroupStatusChatWidgetItem::onMessageDataChanged() {
			m_lblStatus->setText(preprocessLinks(m_groupMessage.getContentAsText()));

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupStatusChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(m_groupMessage.getContentAsText());
		}

	}
}
