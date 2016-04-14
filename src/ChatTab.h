#ifndef OPENMITTSU_CHATTAB_H_
#define OPENMITTSU_CHATTAB_H_

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <cstdint>

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/MessageTime.h"

class Contact;

class ChatTab : public QWidget {
	Q_OBJECT
public:
	ChatTab(QWidget* parent = nullptr) : QWidget(parent) {}
	virtual ~ChatTab() {}

	virtual Contact* getAssociatedContact() const = 0;
public slots:
	virtual void onReceivedMessage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QString const& message) = 0;
	virtual void onReceivedImage(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, QByteArray const& picture) = 0;
	virtual void onReceivedLocation(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId, double latitude, double longitude, double height, QString const& description) = 0;
	virtual void onMessageReceiptReceived(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) = 0;
	virtual void onMessageReceiptSeen(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) = 0;
	virtual void onMessageReceiptAgree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) = 0;
	virtual void onMessageReceiptDisagree(ContactId const& sender, MessageTime const& timeSend, MessageId const& messageId) = 0;
	virtual void onMessageSendFailed(MessageId const& messageId) = 0;
	virtual void onMessageSendDone(MessageId const& messageId) = 0;
	virtual void onUserStartedTypingNotification() = 0;
	virtual void onUserStoppedTypingNotification() = 0;
	virtual void onReceivedFocus() = 0;
	virtual void onContactDataChanged() = 0;
};

#endif // OPENMITTSU_CHATTAB_H_