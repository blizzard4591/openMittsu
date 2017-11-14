#ifndef OPENMITTSU_WIDGETS_CHAT_CHATTABWIDGET_H_
#define OPENMITTSU_WIDGETS_CHAT_CHATTABWIDGET_H_

#include <QTabWidget>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QTimer>
#include <cstdint>

namespace openmittsu {
	namespace widgets {
		class ChatTab;

		class ChatTabWidget : public QTabWidget {
			Q_OBJECT
		public:
			virtual ~ChatTabWidget() {}

			ChatTabWidget(QWidget* parent = nullptr);
			void setTabBlinking(int index, bool doBlink = true);

			void addChatTab(ChatTab* tab);
		public slots:
			void onTabNameChanged(ChatTab* tab);
		signals:
			void hasUnreadMessages();
		private:
			class IconSet {
			public:
				QIcon standardIcon;
				QIcon blinkIcon;
			};
			QHash<int, IconSet> indexToIconHashMap;
			QTimer blinkTimer;
			QColor standardColor;
			QColor blinkColor;
			bool isCurrentlyInBlink;
			int lastActiveIndex;
		private slots:
			void blinkTimerOnTimer();
			void slotCurrentChanged(int index);
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CHATTABWIDGET_H_