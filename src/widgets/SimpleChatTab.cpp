#include "widgets/SimpleChatTab.h"
#include <QListWidgetItem>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImage>
#include <QThread>
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
#include "IdentityContact.h"
#include "GroupContact.h"
#include "exceptions/InternalErrorException.h"
#include "exceptions/NotConnectedException.h"
#include "utility/Logging.h"
#include "utility/QExifImageHeader.h"
#include "utility/QObjectConnectionMacro.h"

#include "ui_simplechattab.h"


SimpleChatTab::SimpleChatTab(Contact* contact, UniqueMessageIdGenerator* idGenerator, QWidget* parent) : ChatTab(parent), uniqueMessageIdGenerator(idGenerator), ui(new Ui::SimpleChatTab), contact(contact), messageIdToItemIndex(), unseenMessages(), writeMessagesToLog(true), messageLogFilename(QString("MessageLog-for-%1.txt").arg(getContactPrintableId(contact))) {
	ui->setupUi(this);

	OPENMITTSU_CONNECT(ui->edtInput, textChanged(), this, edtInputOnTextEdited());
	OPENMITTSU_CONNECT(ui->edtInput, returnPressed(), this, edtInputOnReturnPressed());
	OPENMITTSU_CONNECT(ui->btnInputSend, clicked(), this, btnInputSendOnClick());
	OPENMITTSU_CONNECT(ui->btnSendImage, clicked(), this, btnSendImageOnClick());
	OPENMITTSU_CONNECT(ui->emojiSelector, emojiDoubleClicked(QString const&), this, emojiDoubleClicked(QString const&));

	onContactDataChanged();

	ui->edtInput->setFocus();
}

SimpleChatTab::~SimpleChatTab() {
	//
	delete ui;
}

QString SimpleChatTab::getContactPrintableId(Contact const* const c) {
	if (c == nullptr) {
		return QString("INVALID-PTR");
	}
	if (c->getContactType() == Contact::ContactType::CONTACT_IDENTITY) {
		IdentityContact const* const identityContact = static_cast<IdentityContact const*>(c);
		return identityContact->getContactId().toQString();
	} else {
		GroupContact const* const groupContact = static_cast<GroupContact const*>(c);
		return groupContact->getGroupId().toQString();
	}
}

void SimpleChatTab::writeMessageToLog(QString const& message) {
	QFile logFile(messageLogFilename);
	if (logFile.open(QFile::WriteOnly | QIODevice::Append | QFile::Text)) {
		{
			QTextStream out(&logFile);
			out.setCodec("UTF-8");
			out << message << endl;

			out.flush();
		}
		logFile.close();
	} else {
		LOGGER()->critical("Could not write message log to {}.", messageLogFilename.toStdString());
	}
}

Contact* SimpleChatTab::getAssociatedContact() const {
	return contact;
}

void SimpleChatTab::onReceivedMessage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QString const& message) {
	LOGGER_DEBUG("SimpleChatTab received a text message from {} with ID #{}, sent on {}.", sender.toString(), messageId.toString(), timeSend.toString());
	if (writeMessagesToLog) {
		writeMessageToLog(QString(tr("Received a TEXT message from %1 with ID #%2 sent on %3: %4")).arg(sender.toQString()).arg(messageId.toQString()).arg(timeSend.toQString()).arg(message));
	}

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
	LOGGER_DEBUG("SimpleChatTab received a location message from {} with ID #{}, sent on {}.", sender.toString(), messageId.toString(), timeSend.toString());
	if (writeMessagesToLog) {
		writeMessageToLog(QString(tr("Received a LOCATION message from %1 with ID #%2 sent on %3: Latitude = %4, Longitude = %5, Height = %6, Description = %7")).arg(sender.toQString()).arg(messageId.toQString()).arg(timeSend.toQString()).arg(latitude).arg(longitude).arg(height).arg(description));
	}

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
	LOGGER_DEBUG("SimpleChatTab received an Image of size {} Bytes from {}, sent on {}.", picture.size(), sender.toString(), timeSend.toString());
	if (writeMessagesToLog) {
		writeMessageToLog(QString(tr("Received an IMAGE message from %1 with ID #%2 sent on %3: %4")).arg(sender.toQString()).arg(messageId.toQString()).arg(timeSend.toQString()).arg(QString(picture.toBase64())));
	}

	ContactRegistry* contactRegistry = ContactRegistry::getInstance();

	QPixmap p;
	if (!p.loadFromData(picture)) {
		QString filename(QStringLiteral("failed-image-from-%1.bin"));
		filename = filename.arg(QDateTime::currentDateTime().toString("HH-mm-ss-dd-MM-yyyy"));
		QFile tempFile(filename);
		if (tempFile.open(QFile::WriteOnly)) {
			tempFile.write(picture);
			tempFile.close();
			LOGGER()->critical("Could not load image, wrote raw image to {}.", filename.toStdString());
		} else {
			LOGGER()->critical("Could not load image AND failed to write raw image to {}.", filename.toStdString());
		}
	} else {
		QExifImageHeader header;
		QBuffer buffer;
		buffer.setData(picture);
		buffer.open(QBuffer::ReadOnly);

		QString imageText;
		if (header.loadFromJpeg(&buffer)) {
			if (header.contains(QExifImageHeader::Artist)) {
				LOGGER_DEBUG("Image has Artist Tag: {}", header.value(QExifImageHeader::Artist).toString().toStdString());
				imageText = header.value(QExifImageHeader::Artist).toString();
			} else if (header.contains(QExifImageHeader::UserComment)) {
				LOGGER_DEBUG("Image has UserComment Tag: {}", header.value(QExifImageHeader::UserComment).toString().toStdString());
				imageText = header.value(QExifImageHeader::UserComment).toString();
			} else {
				LOGGER_DEBUG("Image does not have Artist or UserComment Tag.");
			}
		} else {
			LOGGER_DEBUG("Image does not have an EXIF Tag.");
		}

		ImageChatWidgetItem* clwi = new ImageChatWidgetItem(contactRegistry->getIdentity(sender), ContactIdWithMessageId(sender, messageId), timeSend.getTime(), QDateTime::currentDateTime(), p, imageText);
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
		setStatusLine(tr("User received a Message."));
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_RECEIVED, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptSeen(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		setStatusLine(tr("User saw a Message."));
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageState(ChatWidgetItem::MessageState::STATE_READ, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptAgree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		setStatusLine(tr("User agreed with a Message."));
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		if (clwi != nullptr) {
			clwi->setMessageAgreeState(ChatWidgetItem::MessageAgreeState::STATE_AGREE, timeSend.getTime());
		}
	}

	handleFocus();
}

void SimpleChatTab::onMessageReceiptDisagree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		setStatusLine(tr("User disagreed with a Message."));
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
	setStatusLine(tr("User is typing..."));

	handleFocus();
}

void SimpleChatTab::onUserStoppedTypingNotification() {
	setStatusLine(tr("User stopped typing."));

	handleFocus();
}

void SimpleChatTab::onMessageSendFailed(MessageId const& messageId) {
	if (messageIdToItemIndex.contains(messageId)) {
		ChatWidgetItem* clwi = messageIdToItemIndex.value(messageId);
		clwi->setMessageState(ChatWidgetItem::MessageState::STATE_FAILED, QDateTime::currentDateTime());

		if (writeMessagesToLog) {
			writeMessageToLog(QString(tr("Failed to send message with ID #%2.")).arg(messageId.toQString()));
		}
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
			QMessageBox::warning(this, tr("Not connected"), tr("Could not send your message as you are currently not connected to a server."));
			return;
		}

		if (writeMessagesToLog) {
			MessageTime mt = MessageTime::now();
			writeMessageToLog(QString(tr("Send a TEXT message with ID #%2 sent on %3: %4")).arg(messageId.toQString()).arg(mt.toQString()).arg(text));
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
	QString filename = QFileDialog::getOpenFileName(this, tr("Select an Image to Send"), QString(), tr("Images (*.png *.jpg)"));
	if (!filename.isNull() && !filename.isEmpty()) {
		QFile imageFile(filename);
		if (imageFile.open(QFile::ReadOnly)) {
			prepareAndSendImage(imageFile.readAll());
		} else {
			QMessageBox::warning(this, tr("Error loading image"), tr("Could not open selected image.\nInsufficient rights or I/O error."), QMessageBox::Ok, QMessageBox::NoButton);
		}
	}
}

void SimpleChatTab::ctxMenuImageFromUrlOnClick() {
	bool ok = false;
	QString urlString = QInputDialog::getText(this, tr("URL of Image to send"), tr("Please insert the URL of the Image you want to send:"), QLineEdit::Normal, QStringLiteral("http://www.example.com/exampleImage.jpg"), &ok);
	if (ok && !urlString.isEmpty()) {
		FileDownloaderCallbackTask* fileDownloaderCallbackTask = new FileDownloaderCallbackTask(QUrl(urlString));
		OPENMITTSU_CONNECT(fileDownloaderCallbackTask, finished(CallbackTask*), this, fileDownloaderCallbackTaskFinished(CallbackTask*));
		
		QMetaObject::invokeMethod(fileDownloaderCallbackTask, "start", Qt::QueuedConnection);
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
		buffer.open(QIODevice::ReadWrite);
		image.save(&buffer, "JPG", 75);

		// Insert Text if available
		QString const text = ui->edtInput->toPlainText();
		if (!text.isEmpty()) {
			QExifImageHeader header;
			header.setValue(QExifImageHeader::UserComment, QExifValue(text));
			buffer.seek(0);
			header.saveToJpeg(&buffer);
		}

		buffer.close();

		MessageId const messageId = getUniqueMessageId();
		if (!sendImage(messageId, imageBytes)) {
			QMessageBox::warning(this, tr("Not connected"), tr("Could not send your message as you are currently not connected to a server."));
			return;
		}

		if (!text.isEmpty()) {
			ui->edtInput->setPlainText("");
		}

		if (writeMessagesToLog) {
			MessageTime mt = MessageTime::now();
			writeMessageToLog(QString(tr("Send an IMAGE message with ID #%2 sent on %3: %4")).arg(messageId.toQString()).arg(mt.toQString()).arg(QString(imageBytes.toBase64())));
		}

		ImageChatWidgetItem* clwi = new ImageChatWidgetItem(ContactRegistry::getInstance()->getSelfContact(), ContactIdWithMessageId(ContactRegistry::getInstance()->getSelfContact()->getContactId(), messageId), QPixmap::fromImage(image), text);
		ui->chatWidget->addItem(clwi);
		messageIdToItemIndex.insert(messageId, clwi);

	} else {
		QMessageBox::warning(this, tr("Error loading image"), tr("Could not load selected image.\nUnsupported format or I/O error."), QMessageBox::Ok, QMessageBox::NoButton);
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
			QMessageBox::warning(this, tr("Not connected"), tr("Could not send your message as you are currently not connected to a server."));
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

void SimpleChatTab::setStatusLine(QString const& newStatus) {
	static const QString statusTemplate = tr("Status: %1");

	ui->lblStatus->setText(statusTemplate.arg(newStatus));
}

void SimpleChatTab::fileDownloaderCallbackTaskFinished(CallbackTask* callbackTask) {
	if (dynamic_cast<FileDownloaderCallbackTask*>(callbackTask) == nullptr) {
		LOGGER()->warn("SimpleChatTab::fileDownloaderCallbackTaskFinished(CallbackTask* callbackTask) called with a CallbackTask that is not a FileDownloaderCallbackTask. Ignoring.");
		delete callbackTask;
	} else {
		FileDownloaderCallbackTask* fileDownloaderCallbackTask = dynamic_cast<FileDownloaderCallbackTask*>(callbackTask);

		if (!fileDownloaderCallbackTask->hasFinishedSuccessfully()) {
			QMessageBox::warning(this, tr("Download failed"), tr("Downloading from the given URL failed.\nThe error message was:\n").append(fileDownloaderCallbackTask->getErrorMessage()));
		} else {
			prepareAndSendImage(fileDownloaderCallbackTask->getDownloadedFile());
		}

		delete fileDownloaderCallbackTask;
	}
}

void SimpleChatTab::emojiDoubleClicked(QString const& emoji) {
	QTextCursor cursor = ui->edtInput->textCursor();
	cursor.insertText(emoji);
}
