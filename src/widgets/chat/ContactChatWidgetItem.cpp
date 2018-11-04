#include "src/widgets/chat/ContactChatWidgetItem.h"

#include <QAction>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>

#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		ContactChatWidgetItem::ContactChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ChatWidgetItem(message.getContact(), message.isMessageFromUs(), parent), m_contactMessage(message) {
			OPENMITTSU_CONNECT(&m_contactMessage, messageDataChanged(), this, onMessageDataChanged());
			OPENMITTSU_CONNECT(&m_contactMessage, messageDeleted(), this, onMessageDeleted());
		}

		ContactChatWidgetItem::~ContactChatWidgetItem() {
			//
		}

		void ContactChatWidgetItem::showContextMenu(const QPoint& pos) {
			QPoint globalPos = mapToGlobal(pos);
			QMenu listMessagesContextMenu;

			QAction* actionSend = nullptr;
			QAction* actionReceived = nullptr;
			QAction* actionSeen = nullptr;
			QAction* actionAgree = nullptr;
			QAction* actionMessageId = nullptr;
			QAction* actionCopy = nullptr;
			QAction* actionDelete = nullptr;

			openmittsu::protocol::MessageTime const sendTime = m_contactMessage.getSentAt();
			openmittsu::protocol::MessageTime const receivedTime = m_contactMessage.getReceivedAt();
			openmittsu::protocol::MessageTime const seenTime = m_contactMessage.getSeenAt();
			openmittsu::protocol::MessageTime const modifiedTime = m_contactMessage.getModifiedAt();
			openmittsu::dataproviders::messages::UserMessageState const messageState = m_contactMessage.getMessageState();

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


			if (messageState == openmittsu::dataproviders::messages::UserMessageState::USERACK) {
				actionAgree = new QAction(tr("Agreed: %1").arg(modifiedTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else if (messageState == openmittsu::dataproviders::messages::UserMessageState::USERDEC) {
				actionAgree = new QAction(tr("Disagreed: %1").arg(modifiedTime.getTime().toString(QStringLiteral("HH:mm:ss, on dd.MM.yyyy"))), &listMessagesContextMenu);
			} else {
				actionAgree = new QAction(tr("Not agreed/disagreed"), &listMessagesContextMenu);
			}
			listMessagesContextMenu.addAction(actionAgree);

			actionMessageId = new QAction(tr("Message ID: #%1").arg(m_contactMessage.getMessageId().toQString()), &listMessagesContextMenu);
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
					} else if (selectedItem == actionAgree) {
						//
					} else if (selectedItem == actionCopy) {
						copyToClipboard();
					} else if (selectedItem == actionDelete) {
						auto const button = QMessageBox::question(this, tr("Delete selected message?"), tr("Are you sure you want to delete this message?"));
						if (button == QMessageBox::Yes) {
							m_contactMessage.deleteMessage();
						}
					}
				}
			}
		}

		openmittsu::dataproviders::BackedMessage& ContactChatWidgetItem::getMessage() {
			return m_contactMessage;
		}

		openmittsu::dataproviders::BackedMessage const& ContactChatWidgetItem::getMessage() const {
			return m_contactMessage;
		}

	}
}
