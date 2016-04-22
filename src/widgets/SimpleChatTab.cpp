#include "widgets/SimpleChatTab.h"
#include <QListWidgetItem>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImage>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>

#include "KnownIdentities.h"
#include "ChatTabWidget.h"

#include "widgets/TextChatWidgetItem.h"
#include "widgets/ImageChatWidgetItem.h"
#include "widgets/LocationChatWidgetItem.h"

#include "IdentityHelper.h"
#include "MessageCenter.h"
#include "exceptions/InternalErrorException.h"
#include "exceptions/NotConnectedException.h"
#include "tasks/FileDownloader.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"

#include "ui_simplechattab.h"


SimpleChatTab::SimpleChatTab(Contact* contact, UniqueMessageIdGenerator* idGenerator, QWidget* parent) : ChatTab(parent), uniqueMessageIdGenerator(idGenerator), ui(new Ui::SimpleChatTab), contact(contact) {
	ui->setupUi(this);

	OPENMITTSU_CONNECT(ui->edtInput, textChanged(), this, edtInputOnTextEdited());
	OPENMITTSU_CONNECT(ui->edtInput, returnPressed(), this, edtInputOnReturnPressed());
	OPENMITTSU_CONNECT(ui->btnInputSend, clicked(), this, btnInputSendOnClick());
	OPENMITTSU_CONNECT(ui->btnSendImage, clicked(), this, btnSendImageOnClick());

	onContactDataChanged();

	ui->edtInput->setFocus();
}

SimpleChatTab::~SimpleChatTab() {
	//
	delete ui;
}

Contact* SimpleChatTab::getAssociatedContact() const {
	return contact;
}

void SimpleChatTab::onReceivedMessage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QString const& message) {
	LOGGER_DEBUG("SimpleChatTab received a text message from {} with ID #{}, send on {}.", sender.toString(), messageId.toString(), timeSend.toString());
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QDateTime const currentTime = QDateTime::currentDateTime();
	TextChatWidgetItem* clwi = new TextChatWidgetItem(contactRegistry->getIdentity(sender), ContactIdWithMessageId(sender, messageId), timeSend.getTime(), currentTime, message);
	ui->chatWidget->addItem(clwi);
	scrollDownChatWidget();
	messageIdToItemIndex.insert(messageId, clwi);
	//listWidget->scrollToBottom();

	sendReceipt(messageId, ReceiptMessageContent::ReceiptType::RECEIVED);

	// Add to unseen
	unseenMessages.append(messageId);

	handleFocus(true);
}

void SimpleChatTab::onReceivedLocation(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, double latitude, double longitude, double height, QString const& description) {
	LOGGER_DEBUG("SimpleChatTab received a location message from {} with ID #{}, send on {}.", sender.toString(), messageId.toString(), timeSend.toString());
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	LocationChatWidgetItem* lcwi = new LocationChatWidgetItem(contactRegistry->getIdentity(sender), ContactIdWithMessageId(sender, messageId), timeSend.getTime(), QDateTime::currentDateTime(), latitude, longitude, height, description);
	ui->chatWidget->addItem(lcwi);
	scrollDownChatWidget();
	messageIdToItemIndex.insert(messageId, lcwi);
	//listWidget->scrollToBottom();

	sendReceipt(messageId, ReceiptMessageContent::ReceiptType::RECEIVED);

	// Add to unseen
	unseenMessages.append(messageId);

	handleFocus(true);
}

void SimpleChatTab::onReceivedImage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QByteArray const& picture) {
	LOGGER_DEBUG("SimpleChatTab received an Image of size {} Bytes from {}, send on {}.", picture.size(), sender.toString(), timeSend.toString());
	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QPixmap p;
	if (!p.loadFromData(picture)) {
		QString filename(QStringLiteral("failed-image-from-%1.bin"));
		filename = filename.arg(QDateTime::currentDateTime().toString("HH-mm-ss-dd-MM-yyyy"));
		QFile tempFile(filename);
		tempFile.open(QFile::WriteOnly);
		tempFile.write(picture);
		tempFile.close();
		LOGGER()->critical("Could not load image, wrote raw image to {}.", filename.toStdString());
	} else {
		ImageChatWidgetItem* clwi = new ImageChatWidgetItem(contactRegistry->getIdentity(sender), ContactIdWithMessageId(sender, messageId), timeSend.getTime(), QDateTime::currentDateTime(), p);
		ui->chatWidget->addItem(clwi);
		messageIdToItemIndex.insert(messageId, clwi);
		//listWidget->scrollToBottom();

		sendReceipt(messageId, ReceiptMessageContent::ReceiptType::RECEIVED);

		// Add to unseen
		unseenMessages.append(messageId);

		handleFocus(true);
	}
}

void SimpleChatTab::onMessageReceiptReceived(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ui->lblStatus->setText("Status: User received a Message.");
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_RECEIVED, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptSeen(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ui->lblStatus->setText("Status: User saw a Message.");
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_READ, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptAgree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ui->lblStatus->setText("Status: User agreed with a Message.");
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageAgreeState(ChatWidgetItem::MessageAgreeState::STATE_AGREE, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptDisagree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ui->lblStatus->setText("Status: User disagreed with a Message.");
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageAgreeState(ChatWidgetItem::MessageAgreeState::STATE_DISAGREE, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onContactDataChanged() {
	ui->lblDescription->setText(QString(tr("Description: %1")).arg(contact->getContactDescription()));
	ui->chatWidget->onContactDataChanged();
}

void SimpleChatTab::onUserStartedTypingNotification() {
	ui->lblStatus->setText("Status: User is typing...");

	handleFocus();
}

void SimpleChatTab::onUserStoppedTypingNotification() {
	ui->lblStatus->setText("Status: User stopped typing.");

	handleFocus();
}

void SimpleChatTab::onMessageSendFailed(MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		clwi->setMessageState(ChatWidgetItem::MessageState::STATE_FAILED, QDateTime::currentDateTime());
	}

	handleFocus();
}

void SimpleChatTab::onMessageSendDone(MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		LOGGER_DEBUG("Displaying message #{} as sent.", messageId.toString());
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		clwi->setMessageState(ChatWidgetItem::MessageState::STATE_SEND, QDateTime::currentDateTime());

		if (contact->getContactType() == Contact::ContactType::CONTACT_GROUP) {
			// Auto-read the messages to groups
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_RECEIVED, QDateTime::currentDateTime());
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_READ, QDateTime::currentDateTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::btnInputSendOnClick() {
	QString const text = ui->edtInput->toPlainText();

	if (!(text.isEmpty() || text.isNull())) {
		MessageId const messageId = getUniqueMessageId();
		if (!sendText(messageId, text)) {
			QMessageBox::warning(this, "Not connected", "Could not send your message as you are currently not connected to a server.");
			return;
		}

		TextChatWidgetItem* clwi = new TextChatWidgetItem(ContactRegistry::getInstance()->getSelfContact(), ContactIdWithMessageId(ContactRegistry::getInstance()->getSelfContact()->getContactId(), messageId), text);
		ui->chatWidget->addItem(clwi);
		messageIdToItemIndex.insert(messageId, clwi);
		ui->edtInput->setPlainText("");

		if (isTyping) {
			// Send a Stopped_Typing notification
			typingTimerOnTimer();
		}
	}

	handleFocus();
}

void SimpleChatTab::btnSendImageOnClick() {
	QMenu menu;

	QAction* actionFile = new QAction(tr("Load Image from File"), &menu);
	QAction* actionUrl = new QAction(tr("Load Image from URL"), &menu);

	menu.addAction(actionFile);
	menu.addAction(actionUrl);

	// Check Cursor Position:
	// If the cursor is on the button, display the menu there, if not, display the menu on the right side of the button (approx. 0.8 * width).
	QPoint const cursorPosition = QCursor::pos();

	QPoint globalPos = cursorPosition;
	if (!ui->btnSendImage->underMouse()) {
		globalPos = ui->btnSendImage->mapToGlobal(QPoint(ui->btnSendImage->width() * 0.8, ui->btnSendImage->height() / 2));
	}

	QAction* selectedItem = menu.exec(globalPos);
	if (selectedItem != nullptr) {
		if (selectedItem == actionFile) {
			ctxMenuImageFromFileOnClick();
		} else if (selectedItem == actionUrl) {
			ctxMenuImageFromUrlOnClick();
		}
	}
}

void SimpleChatTab::ctxMenuImageFromFileOnClick() {
	QString filename = QFileDialog::getOpenFileName(this, "Select an Image to Send", QString(), "Images (*.png *.jpg)");
	if (!filename.isNull() && !filename.isEmpty()) {
		QFile imageFile(filename);
		if (imageFile.open(QFile::ReadOnly)) {
			prepareAndSendImage(imageFile.readAll());
		} else {
			QMessageBox::warning(this, "Error loading image", "Could not open selected image.\nInsufficient rights or I/O error.", QMessageBox::Ok, QMessageBox::NoButton);
		}
	}
}

void SimpleChatTab::ctxMenuImageFromUrlOnClick() {
	bool ok = false;
	QString urlString = QInputDialog::getText(this, tr("URL of Image to send"), tr("Please insert the URL of the Image you want to send:"), QLineEdit::Normal, QStringLiteral("http://www.example.com/exampleImage.jpg"), &ok);
	if (ok && !urlString.isEmpty()) {
		// Todo
		QMessageBox::information(this, "Not yet implemented!", "Sorry!\nThis feature is not yet implemented.");
	}
}

void SimpleChatTab::prepareAndSendImage(QByteArray const& imageData) {
	QImage image;
	if (image.loadFromData(imageData)) {
		int width = image.width();
		int height = image.height();
		int maxSize = (width >= height) ? width : height;
		if (maxSize > 1500) {
			double factor = 1500.0 / maxSize;
			image = image.scaled(width * factor, height * factor, Qt::AspectRatioMode::KeepAspectRatio, Qt::SmoothTransformation);
		}

		QByteArray imageBytes;
		QBuffer buffer(&imageBytes);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "JPG", 75);
		buffer.close();

		MessageId const messageId = getUniqueMessageId();
		if (!sendImage(messageId, imageBytes)) {
			QMessageBox::warning(this, "Not connected", "Could not send your message as you are currently not connected to a server.");
			return;
		}

		ImageChatWidgetItem* clwi = new ImageChatWidgetItem(ContactRegistry::getInstance()->getSelfContact(), ContactIdWithMessageId(ContactRegistry::getInstance()->getSelfContact()->getContactId(), messageId), QPixmap::fromImage(image));
		ui->chatWidget->addItem(clwi);
		messageIdToItemIndex.insert(messageId, clwi);

	} else {
		QMessageBox::warning(this, "Error loading image", "Could not load selected image.\nUnsupported format or I/O error.", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
}

void SimpleChatTab::edtInputOnReturnPressed() {
	btnInputSendOnClick();
}

void SimpleChatTab::edtInputOnTextEdited() {
	if (!isTyping) {
		isTyping = true;
		if (contact->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
			LOGGER_DEBUG("Sending typing started notification.");
			sendUserTypingStatus(true);
		}
		typingTimer.start(10000);
	} else {
		// Do not reset too often, this does not matter this much
		if (typingTimer.remainingTime() < 9000) {
			typingTimer.stop();
			typingTimer.start(10000);
		}
	}

	handleFocus();
}

void SimpleChatTab::typingTimerOnTimer() {
	if (isTyping) {
		isTyping = false;
		if (contact->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
			LOGGER_DEBUG("Sending typing stopped notification.");
			sendUserTypingStatus(false);
		}
		typingTimer.stop();
	}

	handleFocus();
}

void SimpleChatTab::scrollDownChatWidget() {
	QMetaObject::invokeMethod(ui->chatWidget, "scrollToBottom", Qt::QueuedConnection);
}

void SimpleChatTab::onReceivedFocus() {
	handleFocus();
}

void SimpleChatTab::handleFocus(bool hasNewMessage) {
	ChatTabWidget* tabWidget = dynamic_cast<ChatTabWidget*>(this->parentWidget()->parentWidget());
	if (tabWidget == nullptr) {
		LOGGER()->warn("Ignoring handleFocus() call since the parentWidget is not a ChatTabWidget.");
		return;
	}

	if ((tabWidget->currentWidget() == this) && (QApplication::activeWindow() != nullptr)) {
		QVector<MessageId>::const_iterator it = unseenMessages.constBegin();
		QVector<MessageId>::const_iterator end = unseenMessages.constEnd();

		try {
			for (; it != end; ++it) {
				if (contact->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
					sendReceipt(*it, ReceiptMessageContent::ReceiptType::SEEN);
				}
				if (messageIdToItemIndex.contains(*it)) {
					ChatWidgetItem* clwi = messageIdToItemIndex.value(*it);
					if (clwi != nullptr) {
						clwi->setMessageState(ChatWidgetItem::MessageState::STATE_READ, QDateTime::currentDateTime());
					}
				}
			}
		} catch (NotConnectedException&) {
			QMessageBox::warning(this, "Not connected", "Could not send your message as you are currently not connected to a server.");
			return;
		}
		unseenMessages.clear();

		tabWidget->setTabBlinking(tabWidget->indexOf(this), false);
	} else if (unseenMessages.size() > 0) {
		tabWidget->setTabBlinking(tabWidget->indexOf(this), true);
		if (hasNewMessage) {
			MessageCenter::getInstance()->chatTabHasNewUnreadMessageAvailable(this);
		}
	}
}
