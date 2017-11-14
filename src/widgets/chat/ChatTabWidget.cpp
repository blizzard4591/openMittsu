#include "src/widgets/chat/ChatTabWidget.h"

#include <QPalette>
#include <QPainter>
#include <QTabBar>

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/widgets/chat/ChatTab.h"

namespace openmittsu {
	namespace widgets {

		ChatTabWidget::ChatTabWidget(QWidget* parent) : QTabWidget(parent), standardColor(this->palette().color(QPalette::Foreground)), blinkColor(Qt::red), lastActiveIndex(-1) {
			OPENMITTSU_CONNECT(&blinkTimer, timeout(), this, blinkTimerOnTimer());
			OPENMITTSU_CONNECT(this, currentChanged(int), this, slotCurrentChanged(int));
		}

		void ChatTabWidget::setTabBlinking(int index, bool doBlink) {
			if (doBlink) {
				if (!indexToIconHashMap.contains(index)) {
					QIcon const& icon = this->tabBar()->tabIcon(index);
					IconSet iconSet;
					iconSet.standardIcon = icon;

					// Use the current icon, blend it and use that as blink-icon
					QImage standardIconImage = icon.pixmap(tabBar()->iconSize()).toImage();

					QPainter painter(&standardIconImage);
					painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);

					// Use a white image in half transparent mode for blending
					QPixmap overlay(standardIconImage.size());
					overlay.fill(QColor(255, 255, 255, 127));
					painter.drawPixmap(0, 0, overlay);
					painter.end();

					iconSet.blinkIcon = QIcon(QPixmap::fromImage(standardIconImage));
					indexToIconHashMap.insert(index, iconSet);

					if (indexToIconHashMap.size() == 1) {
						isCurrentlyInBlink = false;
						blinkTimer.start(750);
					}
				}
			} else {
				if (indexToIconHashMap.contains(index)) {
					this->tabBar()->setTabTextColor(index, standardColor);
					indexToIconHashMap.remove(index);
					if (indexToIconHashMap.isEmpty()) {
						blinkTimer.stop();
					}
				}
			}
		}

		void ChatTabWidget::blinkTimerOnTimer() {
			QList<int> indices = indexToIconHashMap.keys();
			QList<int>::const_iterator it = indices.constBegin();
			QList<int>::const_iterator end = indices.constEnd();
			for (; it != end; ++it) {
				if (isCurrentlyInBlink) {
					this->tabBar()->setTabTextColor(*it, standardColor);
					this->tabBar()->setTabIcon(*it, indexToIconHashMap.find(*it).value().standardIcon);
				} else {
					this->tabBar()->setTabTextColor(*it, blinkColor);
					this->tabBar()->setTabIcon(*it, indexToIconHashMap.find(*it).value().blinkIcon);
				}
			}

			isCurrentlyInBlink = !isCurrentlyInBlink;
			blinkTimer.start();
		}

		void ChatTabWidget::addChatTab(ChatTab* tab) {
			if (tab != nullptr) {
				this->addTab(tab, tab->getTabName());
				OPENMITTSU_CONNECT(tab, tabNameChanged(ChatTab*), this, onTabNameChanged(ChatTab*));
			}
		}

		void ChatTabWidget::onTabNameChanged(ChatTab* tab) {
			if (tab == nullptr) {
				return;
			}
			int index = this->indexOf(tab);
			if (index > -1) {
				this->setTabText(index, tab->getTabName());
			}
		}

		void ChatTabWidget::slotCurrentChanged(int index) {
			if (index != -1) {
				ChatTab* const chatTab = dynamic_cast<ChatTab*>(this->widget(index));
				if (chatTab != nullptr) {
					chatTab->onReceivedFocus();
				}

				if (lastActiveIndex != -1) {
					ChatTab* const lastChatTab = dynamic_cast<ChatTab*>(this->widget(lastActiveIndex));
					if (lastChatTab != nullptr) {
						lastChatTab->onLostFocus();
					}
				}
			}
			lastActiveIndex = index;
		}

	}
}
