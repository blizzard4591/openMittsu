#ifndef OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_

#include <QHash>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <cstdint>
#include "ChatTab.h"
#include "ContactRegistry.h"
#include "messages/contact/ReceiptMessageContent.h"
#include "protocol/UniqueMessageIdGenerator.h"
#include "tasks/FileDownloaderCallbackTask.h"
#include "widgets/ChatWidget.h"

namespace Ui {
class SimpleChatTab;
}

class SimpleChatTab : public ChatTab {
	Q_OBJECT
public:
	explicit SimpleChatTab(Contact* contact, UniqueMessageIdGenerator* idGenerator, QWidget *parent = 0);
    virtual ~SimpleChatTab();

	virtual Contact* getAssociatedContact() const override;

	virtual void onReceivedMessage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QString const& message) override;
	virtual void onReceivedImage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QByteArray const& picture) override;
	virtual void onReceivedLocation(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, double latitude, double longitude, double height, QString const& description) override;
	virtual void onMessageReceiptReceived(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) override;
	virtual void onMessageReceiptSeen(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) override;
	virtual void onMessageReceiptAgree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) override;
	virtual void onMessageReceiptDisagree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) override;
	virtual void onUserStartedTypingNotification() override;
	virtual void onUserStoppedTypingNotification() override;
	virtual void onReceivedFocus() override;
	virtual void onContactDataChanged() override;
	virtual void onMessageSendFailed(MessageId const& messageId) override;
	virtual void onMessageSendDone(MessageId const& messageId) override;
public slots:
	void btnInputSendOnClick();
	void btnSendImageOnClick();

	void ctxMenuImageFromFileOnClick();
	void ctxMenuImageFromUrlOnClick();

	void edtInputOnReturnPressed();
	void edtInputOnTextEdited();
	void typingTimerOnTimer();
	void scrollDownChatWidget();

	void emojiDoubleClicked(QString const& emoji);

	void fileDownloaderCallbackTaskFinished(CallbackTask* callbackTask);
protected:
	virtual bool sendText(MessageId const& uniqueMessageId, QString const& text) = 0;
	virtual bool sendImage(MessageId const& uniqueMessageId, QByteArray const& image) = 0;
	virtual bool sendUserTypingStatus(bool isTyping) = 0;
	virtual bool sendReceipt(MessageId const& receiptedMessageId, ReceiptMessageContent::ReceiptType const& receiptType) = 0;
	virtual MessageId getUniqueMessageId() = 0;
	virtual void setStatusLine(QString const& newStatus);

	static QString getContactPrintableId(Contact const* const c);
	virtual void writeMessageToLog(QString const& message);

	UniqueMessageIdGenerator* const uniqueMessageIdGenerator;
private:
	Ui::SimpleChatTab *ui;
	Contact* const contact;
	QHash<MessageId, ChatWidgetItem*> messageIdToItemIndex;
	QVector<MessageId> unseenMessages;

	// Should we write a Log?
	bool writeMessagesToLog;
	QString messageLogFilename;

	// Handling for Typing Notifications
	bool isTyping;
	QTimer typingTimer;

	void handleFocus(bool hasNewMessage = false);
	void prepareAndSendImage(QByteArray const& imageData);
};

#endif // OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_
