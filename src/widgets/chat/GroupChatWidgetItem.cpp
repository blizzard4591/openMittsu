#include "src/widgets/chat/GroupChatWidgetItem.h"

#include <QAction>
#include <QClipboard>
#include <QMessageBox>

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
			QAction* actionDelete = nullptr;

			openmittsu::protocol::MessageTime const sendTime = m_groupMessage.getSentAt();
			openmittsu::protocol::MessageTime const receivedTime = m_groupMessage.getReceivedAt();
			openmittsu::protocol::MessageTime const seenTime = m_groupMessage.getSeenAt();

			if (!sendTime.isNull()) {
				actionSend = new QAction(tr("Sent: %1").arg(sendTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionSend = new QAction(tr("Sent: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionSend);
			if (!receivedTime.isNull()) {
				actionReceived = new QAction(tr("Received: %1").arg(receivedTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionReceived = new QAction(tr("Received: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionReceived);
			if (!seenTime.isNull()) {
				actionSeen = new QAction(tr("Seen: %1").arg(seenTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionSeen = new QAction(tr("Seen: -unknown-"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionSeen);

			actionMessageId = new QAction(tr("Message ID: #%1").arg(m_groupMessage.getMessageId().toQString()), &listMessagesContextMenu);
			listMessagesContextMenu.addAction(actionMessageId);

			actionCopy = new QAction(tr("Copy to Clipboard"), &listMessagesContextMenu);
			listMessagesContextMenu.addAction(actionCopy);

			actionDelete = new QAction(tr("Delete Message"), &listMessagesContextMenu);
			listMessagesContextMenu.addAction(actionDelete);

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
					} else if (selectedItem == actionDelete) {
						auto const button = QMessageBox::question(this, tr("Delete selected message?"), tr("Are you sure you want to delete this message?"));
						if (button == QMessageBox::Yes) {
							m_groupMessage.deleteMessage();
						}
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
