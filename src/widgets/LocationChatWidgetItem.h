#ifndef OPENMITTSU_WIDGETS_LOCATIONCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_LOCATIONCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

namespace Ui {
	class LocationChatWidgetItem;
}

class LocationChatWidgetItem : public ChatWidgetItem {
    Q_OBJECT
public:
	explicit LocationChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, double latitude, double longitude, double height, QString const& description, QWidget *parent = nullptr);
	explicit LocationChatWidgetItem(Contact* contact, ContactIdWithMessageId const& senderAndMessageId, QDateTime const& timeSend, QDateTime const& timeReceived, double latitude, double longitude, double height, QString const& description, QWidget *parent = nullptr);
	~LocationChatWidgetItem();

	virtual void setInnerAlignment(bool alignLeft) override;
	virtual void resizeEvent(QResizeEvent * event) override;
	virtual int heightForWidth(int w) const override;
	virtual bool hasHeightForWidth() const;
protected:
	virtual void setFromTimeString(QString const& text) override;
	virtual void copyToClipboard() override;
private:
	Ui::LocationChatWidgetItem *ui;
	double const latitude;
	double const longitude;
	double const height;
	QString const description;
};

#endif // OPENMITTSU_WIDGETS_LOCATIONCHATWIDGETITEM_H_
