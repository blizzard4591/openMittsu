#include "src/widgets/SimpleChatTab.h"
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

#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/NotConnectedException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/protocol/TextLengthLimiter.h"

#include "ui_simplechattab.h"

namespace openmittsu {
	namespace widgets {

		SimpleChatTab::SimpleChatTab(QWidget* parent) : ChatTab(parent), m_ui(new Ui::SimpleChatTab), m_isTyping(false) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(m_ui->edtInput, textChanged(), this, edtInputOnTextEdited());
			OPENMITTSU_CONNECT(m_ui->edtInput, returnPressed(), this, edtInputOnReturnPressed());
			OPENMITTSU_CONNECT(m_ui->btnInputSend, clicked(), this, btnInputSendOnClick());
			OPENMITTSU_CONNECT(m_ui->btnSendImage, clicked(), this, btnSendImageOnClick());
			OPENMITTSU_CONNECT(m_ui->btnMenu, clicked(), this, btnMenuOnClick());
			OPENMITTSU_CONNECT(m_ui->emojiSelector, emojiDoubleClicked(QString const&), this, emojiDoubleClicked(QString const&));
			OPENMITTSU_CONNECT(&m_typingTimer, timeout(), this, typingTimerOnTimer());

			m_ui->edtInput->setFocus();

			// Load the last 10 messages
			QTimer::singleShot(1, this, SLOT(loadLastNMessages()));
		}

		SimpleChatTab::~SimpleChatTab() {
			//
			delete m_ui;
		}

		void SimpleChatTab::loadLastNMessages() {
			QVector<QString> const lastMessages = getMessageSource().getLastMessageUuids(10u);
			for (int i = 0; i < lastMessages.size(); ++i) {
				QString const& messageUuid = lastMessages.at(i);
				this->onNewMessage(messageUuid);
			}
		}

		void SimpleChatTab::addChatWidgetItem(ChatWidgetItem* item) {
			this->m_ui->chatWidget->addItem(item);
		}

		void SimpleChatTab::btnInputSendOnClick() {
			QString const text = m_ui->edtInput->toPlainText();

			if (!(text.isEmpty() || text.isNull())) {
				QStringList messageParts = openmittsu::protocol::TextLengthLimiter::splitTextForSending(text);

				auto it = messageParts.constBegin();
				auto end = messageParts.constEnd();
				for (; it != end; ++it) {
					sendText(*it);

					LOGGER_DEBUG("Sending text with {} Bytes.", it->toUtf8().size());
				}

				m_ui->edtInput->setPlainText("");

				if (m_isTyping) {
					// Send a Stopped_Typing notification
					typingTimerOnTimer();
				}
			}
		}

		void SimpleChatTab::btnMenuOnClick() {
			QMenu menu;

			QAction* actionLoad25 = new QAction(tr("Load 25 more messages"), &menu);
			QAction* actionLoadN = new QAction(tr("Load N more messages..."), &menu);

			menu.addAction(actionLoad25);
			menu.addAction(actionLoadN);

			// Check Cursor Position:
			// If the cursor is on the button, display the menu there, if not, display the menu on the right side of the button (approx. 0.8 * width).
			QPoint const cursorPosition = QCursor::pos();

			QPoint globalPos = cursorPosition;
			if (!m_ui->btnMenu->underMouse()) {
				globalPos = m_ui->btnMenu->mapToGlobal(QPoint(m_ui->btnMenu->width() * 0.8, m_ui->btnMenu->height() / 2));
			}

			QAction* selectedItem = menu.exec(globalPos);
			if (selectedItem != nullptr) {
				if (selectedItem == actionLoad25) {
					QVector<QString> const lastMessages = getMessageSource().getLastMessageUuids(static_cast<std::size_t>(25 + m_knownUuids.size()));
					for (int i = 0; i < lastMessages.size(); ++i) {
						QString const& messageUuid = lastMessages.at(i);
						this->onNewMessage(messageUuid);
					}
				} else if (selectedItem == actionLoadN) {
					bool ok = false;
					int const result = QInputDialog::getInt(this, tr("Choose how many messages to load"), tr("Please choose how many older messages should be loaded:"), 25, 1, 1000, 1, &ok);
					if (ok) {
						QVector<QString> const lastMessages = getMessageSource().getLastMessageUuids(static_cast<std::size_t>(result + m_knownUuids.size()));
						for (int i = 0; i < lastMessages.size(); ++i) {
							QString const& messageUuid = lastMessages.at(i);
							this->onNewMessage(messageUuid);
						}
					}
				}
			}
		}

		void SimpleChatTab::btnSendImageOnClick() {
			QMenu menu;

			QAction* actionClipboard = new QAction(tr("Load Image from Clipboard"), &menu);
			QAction* actionFile = new QAction(tr("Load Image from File"), &menu);
			QAction* actionUrl = new QAction(tr("Load Image from URL"), &menu);

			menu.addAction(actionClipboard);
			menu.addAction(actionFile);
			menu.addAction(actionUrl);

			// Check Cursor Position:
			// If the cursor is on the button, display the menu there, if not, display the menu on the right side of the button (approx. 0.8 * width).
			QPoint const cursorPosition = QCursor::pos();

			QPoint globalPos = cursorPosition;
			if (!m_ui->btnSendImage->underMouse()) {
				globalPos = m_ui->btnSendImage->mapToGlobal(QPoint(m_ui->btnSendImage->width() * 0.8, m_ui->btnSendImage->height() / 2));
			}

			QAction* selectedItem = menu.exec(globalPos);
			if (selectedItem != nullptr) {
				if (selectedItem == actionFile) {
					ctxMenuImageFromFileOnClick();
				} else if (selectedItem == actionUrl) {
					ctxMenuImageFromUrlOnClick();
				} else if (selectedItem == actionClipboard) {
					ctxMenuImageFromClipboardOnClick();
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
				openmittsu::tasks::FileDownloaderCallbackTask* fileDownloaderCallbackTask = new openmittsu::tasks::FileDownloaderCallbackTask(QUrl(urlString));
				OPENMITTSU_CONNECT(fileDownloaderCallbackTask, finished(openmittsu::tasks::CallbackTask*), this, fileDownloaderCallbackTaskFinished(openmittsu::tasks::CallbackTask*));

				QMetaObject::invokeMethod(fileDownloaderCallbackTask, "start", Qt::QueuedConnection);
			}
		}

		void SimpleChatTab::ctxMenuImageFromClipboardOnClick() {
			QClipboard *clipboard = QApplication::clipboard();

			QImage clipboardImage = clipboard->image();
			if (!clipboardImage.isNull()) {
				prepareAndSendImage(clipboardImage);
			}
		}

		void SimpleChatTab::prepareAndSendImage(QImage const& image) {
			QImage resizedImage;
			int width = image.width();
			int height = image.height();
			int maxSize = std::max(width, height);
			if (maxSize > 1500) {
				double factor = 1500.0 / maxSize;
				resizedImage = image.scaled(width * factor, height * factor, Qt::AspectRatioMode::KeepAspectRatio, Qt::SmoothTransformation);
			} else {
				resizedImage = image;
			}

			QByteArray imageBytes;
			QBuffer buffer(&imageBytes);
			buffer.open(QIODevice::ReadWrite);
			resizedImage.save(&buffer, "JPG", 75);
			buffer.close();

			QString const text = m_ui->edtInput->toPlainText();
			sendImage(imageBytes, text);

			if (!text.isEmpty()) {
				m_ui->edtInput->setPlainText("");
			}
		}

		void SimpleChatTab::prepareAndSendImage(QByteArray const& imageData) {
			QImage image;
			if (image.loadFromData(imageData)) {
				prepareAndSendImage(image);
			} else {
				QMessageBox::warning(this, tr("Error loading image"), tr("Could not load selected image.\nUnsupported format or I/O error."), QMessageBox::Ok, QMessageBox::NoButton);
			}
		}

		void SimpleChatTab::edtInputOnReturnPressed() {
			btnInputSendOnClick();
		}

		void SimpleChatTab::edtInputOnTextEdited() {
			if (!m_isTyping) {
				m_isTyping = true;
				sendUserTypingStatus(true);
				m_typingTimer.start(10000);
			} else {
				// Do not reset too often, this does not matter this much
				if (m_typingTimer.remainingTime() < 9000) {
					m_typingTimer.stop();
					m_typingTimer.start(10000);
				}
			}
		}

		void SimpleChatTab::typingTimerOnTimer() {
			if (m_isTyping) {
				m_isTyping = false;
				sendUserTypingStatus(false);
				m_typingTimer.stop();
			}
		}

		void SimpleChatTab::scrollDownChatWidget() {
			QMetaObject::invokeMethod(m_ui->chatWidget, "scrollToBottom", Qt::QueuedConnection);
		}

		void SimpleChatTab::internalOnReceivedFocus() {
			m_ui->chatWidget->setIsActive(true);
		}

		void SimpleChatTab::internalOnLostFocus() {
			m_ui->chatWidget->setIsActive(false);
		}

		void SimpleChatTab::setStatusLine(QString const& newStatus) {
			m_ui->lblStatus->setText(newStatus);
		}

		void SimpleChatTab::setDescriptionLine(QString const& newDescription) {
			m_ui->lblDescription->setText(newDescription);
		}

		void SimpleChatTab::setMessageCount(int messageCount) {
			m_ui->lblMessageCount->setText(tr("Stored: %1 messages.").arg(messageCount));
		}

		void SimpleChatTab::fileDownloaderCallbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask) {
			if (dynamic_cast<openmittsu::tasks::FileDownloaderCallbackTask*>(callbackTask) == nullptr) {
				LOGGER()->warn("SimpleChatTab::fileDownloaderCallbackTaskFinished(CallbackTask* callbackTask) called with a CallbackTask that is not a FileDownloaderCallbackTask. Ignoring.");
				delete callbackTask;
			} else {
				openmittsu::tasks::FileDownloaderCallbackTask* fileDownloaderCallbackTask = dynamic_cast<openmittsu::tasks::FileDownloaderCallbackTask*>(callbackTask);

				if (!fileDownloaderCallbackTask->hasFinishedSuccessfully()) {
					QMessageBox::warning(this, tr("Download failed"), tr("Downloading from the given URL failed.\nThe error message was:\n").append(fileDownloaderCallbackTask->getErrorMessage()));
				} else {
					prepareAndSendImage(fileDownloaderCallbackTask->getDownloadedFile());
				}

				delete fileDownloaderCallbackTask;
			}
		}

		void SimpleChatTab::emojiDoubleClicked(QString const& emoji) {
			QTextCursor cursor = m_ui->edtInput->textCursor();
			cursor.insertText(emoji);
		}

	}
}
