#ifndef OPENMITTSU_WIDGETS_CHAT_CHATWIDGET_H_
#define OPENMITTSU_WIDGETS_CHAT_CHATWIDGET_H_

#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>

#include "src/widgets/chat/ChatWidgetItem.h"

namespace Ui {
class ChatWidget;
}

namespace openmittsu {
	namespace widgets {

		class ChatWidget : public QScrollArea {
			Q_OBJECT
		public:
			explicit ChatWidget(QWidget* parent = nullptr);
			virtual ~ChatWidget();

			void addItem(ChatWidgetItem* item);
		public slots:
			void scrollToBottom();
			void setIsActive(bool isActive);
		signals:
			void hasUnreadMessages();
		protected:
			virtual void resizeEvent(QResizeEvent* event) override;
		private:
			Ui::ChatWidget* m_ui;
			QVBoxLayout* m_topLayout;
			QVector<ChatWidgetItem*> m_items;
			bool m_isActive;

			// Unread messages
			bool m_hasUnreadMessage;
			QTimer m_unreadMessagesTimer;

			void informAllOfSize();
			void markMessagesAsRead();
		private slots:
			void onUnreadMessagesTimerExpired();
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CHATWIDGET_H_
