#include "GroupChatWidgetItem.h"

#include <QMenu>
#include <QAction>
#include <QClipboard>

#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		GroupChatWidgetItem::GroupChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : ChatWidgetItem(message.getContact(), message.isMessageFromUs(), parent), m_groupMessage(message) {
			OPENMITTSU_CONNECT(&m_groupMessage, messageDataChanged(), this, onMessageDataChanged());
		}

		GroupChatWidgetItem::~GroupChatWidgetItem() {
			//
		}

		void GroupChatWidgetItem::showContextMenu(const QPoint& pos) {
			QPoint globalPos = mapToGlobal(pos);
			QMenu listMessagesContextMenu;

			QAction* actionSend = nullptr;
			QAction* actionReceived = nullptr;
			QAction* actionSeen = nullptr;
			QAction* actionMessageId = nullptr;
			QAction* actionCopy = nullptr;

			openmittsu::protocol::MessageTime const sendTime = m_groupMessage.getSentAt();
			openmittsu::protocol::MessageTime const receivedTime = m_groupMessage.getReceivedAt();
			openmittsu::protocol::MessageTime const seenTime = m_groupMessage.getSeenAt();

			if (!sendTime.isNull()) {
				actionSend = new QAction(QString("Sent: %1").arg(sendTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionSend = new QAction(QString("Sent: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionSend);
			if (!receivedTime.isNull()) {
				actionReceived = new QAction(QString("Received: %1").arg(receivedTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionReceived = new QAction(QString("Received: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionReceived);
			if (!seenTime.isNull()) {
				actionSeen = new QAction(QString("Seen: %1").arg(seenTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionSeen = new QAction(QString("Seen: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionSeen);

			actionMessageId = new QAction(QString("Message ID: #%1").arg(m_groupMessage.getMessageId().toQString()), &listMessagesContextMenu);
			listMessagesContextMenu.addAction(actionMessageId);

			actionCopy = new QAction(QString("Copy to Clipboard"), &listMessagesContextMenu);
			listMessagesContextMenu.addAction(actionCopy);

			appendCustomContextMenuEntries(pos, listMessagesContextMenu);

			QAction* selectedItem = listMessagesContextMenu.exec(globalPos);
			if (selectedItem != nullptr) {
				if (!handleCustomContextMenuEntrySelection(selectedItem)) {
					if (selectedItem == actionSend) {
						//
					} else if (selectedItem == actionSeen) {
						//
					} else if (selectedItem == actionReceived) {
						//
					} else if (selectedItem == actionCopy) {
						copyToClipboard();
					}
				}
			}
		}

		openmittsu::dataproviders::BackedMessage& GroupChatWidgetItem::getMessage() {
			return m_groupMessage;
		}

		openmittsu::dataproviders::BackedMessage const& GroupChatWidgetItem::getMessage() const {
			return m_groupMessage;
		}

	}
}
