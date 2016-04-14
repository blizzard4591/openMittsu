#ifndef OPENMITTSU_WIDGETS_CHATWIDGET_H_
#define OPENMITTSU_WIDGETS_CHATWIDGET_H_

#include <QScrollArea>
#include <QVBoxLayout>
#include <QVector>

#include "ChatWidgetItem.h"
#include "TextChatWidgetItem.h"
#include "ImageChatWidgetItem.h"

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();

	void addItem(ChatWidgetItem* item);
public slots:
	void onContactDataChanged();
	void scrollToBottom();
protected:
	virtual void resizeEvent(QResizeEvent* event) override;
private:
    Ui::ChatWidget *ui;
	QVBoxLayout* topLayout;
	//QWidget* contents;
	QVector<ChatWidgetItem*> items;
	
	void informAllOfSize();
};

#endif // OPENMITTSU_WIDGETS_CHATWIDGET_H_
