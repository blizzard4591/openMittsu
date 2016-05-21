#ifndef OPENMITTSU_WIDGETS_TEXTCHATITEM_H_
#define OPENMITTSU_WIDGETS_TEXTCHATITEM_H_

#include "ChatWidgetItem.h"

namespace Ui {
	class TextChatWidgetItem;
}

class TextChatWidgetItem : public ChatWidgetItem {
    Q_OBJECT
public:
	explicit TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QString const& message, QWidget *parent = nullptr);
	explicit TextChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QString const& message, QWidget *parent = nullptr);
	~TextChatWidgetItem();

	virtual void setInnerAlignment(bool alignLeft) override;
	virtual void resizeEvent(QResizeEvent * event) override;
	virtual int heightForWidth(int w) const override;
	virtual bool hasHeightForWidth() const;
protected:
	virtual void setFromString(QString const& fromString) override;
	virtual void setTimeAndStatusString(QString const& timeAndStatusString) override;

	virtual void copyToClipboard() override;
private:
	Ui::TextChatWidgetItem *ui;
	QString const message;
};

#endif // OPENMITTSU_WIDGETS_TEXTCHATITEM_H_
