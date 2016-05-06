#ifndef OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QMenu>

#include <cstdint>
#include "Contact.h"

#include "protocol/ContactId.h"
#include "protocol/MessageId.h"
#include "protocol/ContactIdWithMessageId.h"

class ChatWidgetItem : public QWidget {
	Q_OBJECT
public:
	enum class MessageState {
		STATE_ENQUEUED,
		STATE_FAILED,
		STATE_SEND,
		STATE_RECEIVED,
		STATE_READ
	};

	enum class MessageAgreeState {
		STATE_NONE,
		STATE_AGREE,
		STATE_DISAGREE
	};

	enum class MessageType {
		TYPE_LOCATION,
		TYPE_TEXT,
		TYPE_VIDEO,
		TYPE_VOICE,
		TYPE_PICTURE
	};

	ChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QWidget* parent = nullptr);
	ChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QWidget* parent = nullptr);
	virtual ~ChatWidgetItem() {
	}

	Contact* getContact() const;
	ContactIdWithMessageId const& getSenderAndMessageId() const;
	QDateTime const& getMessageTimeSend() const;
	QDateTime const& getMessageTimeReceived() const;
	QDateTime const& getMessageTimeSeen() const;
	QDateTime const& getMessageTimeAgree() const;
	MessageState const& getMessageState() const;
	MessageAgreeState const& getMessageAgreeState() const;
	virtual MessageType getMessageType() const;

	void setMessageState(MessageState const& state, QDateTime const& when);
	void setMessageAgreeState(MessageAgreeState const& state, QDateTime const& when);

	virtual void setInnerAlignment(bool alignLeft) = 0;
	virtual QSize sizeHint() const override;
public slots:
	virtual void showContextMenu(const QPoint& pos);
	virtual void onDataUpdated();
	virtual void setOptimalSizeHint(int width);
protected:
	Contact* const contact;
	ContactIdWithMessageId const senderAndMessageId;
	bool hasTimeSendSet;
	QDateTime timeSend;
	bool hasTimeReceivedSet;
	QDateTime timeReceived;
	bool hasTimeSeenSet;
	QDateTime timeSeen;
	bool hasTimeAgreeSet;
	QDateTime timeAgree;
	MessageState messageState;
	MessageAgreeState messageAgreeState;
	MessageType const messageType;

	QString getFormattedContactName() const;
	QSize mySizeHint;

	virtual void setFromTimeString(QString const& text) = 0;
	virtual void appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu);
	virtual bool handleCustomContextMenuEntrySelection(QAction* selectedAction);
	virtual void copyToClipboard();

	QString preprocessLinks(QString const& text);
};

#endif // OPENMITTSU_WIDGETS_CHATWIDGETITEM_H_