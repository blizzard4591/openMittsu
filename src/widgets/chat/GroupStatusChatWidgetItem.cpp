#include "GroupStatusChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "src/exceptions/InternalErrorException.h"
#include "src/protocol/ContactIdList.h"

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
			auto const messageType = m_groupMessage.getMessageType();
			switch (messageType) {
				case openmittsu::dataproviders::messages::GroupMessageType::SET_IMAGE:
					m_lblStatus->setText(tr("&lt;&lt;&lt; The group photo was set/sent by the Group Admin. &gt;&gt;&gt;"));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::SET_TITLE:
					m_lblStatus->setText(tr("&lt;&lt;&lt; The group title was set/sent by the Group Admin: %1 &gt;&gt;&gt;").arg(m_groupMessage.getContentAsText()));
					break;
				case openmittsu::dataproviders::messages::GroupMessageType::GROUP_CREATION:
				{
					QString const members = openmittsu::protocol::ContactIdList::fromString(m_groupMessage.getContentAsText()).toStringS();
					m_lblStatus->setText(tr("&lt;&lt;&lt; Group creation and membership information was set/sent by the Group Admin: %1 &gt;&gt;&gt;").arg(members));
					break;
				}
				case openmittsu::dataproviders::messages::GroupMessageType::SYNC_REQUEST:
					m_lblStatus->setText(tr("&lt;&lt;&lt; Contact %1 requested a group sync. &gt;&gt;&gt;").arg(m_groupMessage.getSender().toQString()));
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Error: Unhandled GroupMessageType " << openmittsu::dataproviders::messages::GroupMessageTypeHelper::toString(messageType) << "!";
			}

			GroupChatWidgetItem::onMessageDataChanged();
		}

		void GroupStatusChatWidgetItem::copyToClipboard() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(m_groupMessage.getContentAsText());
		}

	}
}
