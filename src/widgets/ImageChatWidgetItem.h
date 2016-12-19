#ifndef OPENMITTSU_WIDGETS_IMAGECHATITEM_H_
#define OPENMITTSU_WIDGETS_IMAGECHATITEM_H_

#include "ChatWidgetItem.h"
#include <QPixmap>
#include <QString>

namespace Ui {
	class ImageChatWidgetItem;
}

class ImageChatWidgetItem : public ChatWidgetItem {
    Q_OBJECT
public:
	explicit ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QPixmap const& image, QString const& imageText, QWidget *parent = nullptr);
	explicit ImageChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, QPixmap const& image, QString const& imageText, QWidget *parent = nullptr);
	~ImageChatWidgetItem();

	virtual void setInnerAlignment(bool alignLeft) override;
protected:
	virtual void setFromString(QString const& fromString) override;
	virtual void setTimeAndStatusString(QString const& timeAndStatusString) override;
	virtual void copyToClipboard() override;
private slots:
	void lblImageOnClick();
private:
	Ui::ImageChatWidgetItem *ui;
	QPixmap const image;
	QString const imageText;
};

#endif // OPENMITTSU_WIDGETS_IMAGECHATITEM_H_
