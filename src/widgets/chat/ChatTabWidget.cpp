#include "src/widgets/chat/ChatTabWidget.h"

#include <QPalette>
#include <QPainter>
#include <QTabBar>

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "src/widgets/chat/ChatTab.h"

namespace openmittsu {
	namespace widgets {

		ChatTabWidget::ChatTabWidget(QWidget* parent) : QTabWidget(parent), standardColor(this->palette().color(QPalette::WindowText)), blinkColor(Qt::red), isCurrentlyInBlink(false), lastActiveIndex(-1) {
			OPENMITTSU_CONNECT(&blinkTimer, timeout(), this, blinkTimerOnTimer());
			OPENMITTSU_CONNECT(this, currentChanged(int), this, slotCurrentChanged(int));

			// Make closable
			this->setTabsClosable(true);
		}

		void ChatTabWidget::setTabBlinking(int index, bool doBlink) {
			if (doBlink) {
				if (!indexToIconHashMap.contains(index)) {
					QIcon const icon = this->tabBar()->tabIcon(index);
					IconSet iconSet;
					iconSet.standardIcon = icon;
					
					// Use the current icon, blend it and use that as blink-icon
					QImage const standardIconImageBase = icon.pixmap(tabBar()->iconSize()).toImage();
					QImage standardIconImage = standardIconImageBase.convertToFormat(QImage::Format_ARGB32_Premultiplied);

					if (!icon.isNull()) {
						QPainter painter;
						if (!painter.begin(&standardIconImage)) {
							LOGGER()->warn("Failed to get painter for tab icon, size = {} x {}, null = {}!", tabBar()->iconSize().height(), tabBar()->iconSize().width(), standardIconImageBase.isNull());
						} else {
							painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);

							// Use a white image in half transparent mode for blending
							QPixmap overlay(standardIconImage.size());
							overlay.fill(QColor(255, 255, 255, 127));
							painter.drawPixmap(0, 0, overlay);
							painter.end();
						}
					}

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
				OPENMITTSU_CONNECT(tab, hasUnreadMessages(ChatTab*), this, onTabHasUnreadMessages(ChatTab*));
			}
		}

		void ChatTabWidget::onTabNameChanged(ChatTab* tab) {
			if (tab == nullptr) {
				return;
			}
			auto const index = this->indexOf(tab);
			if (index > -1) {
				this->setTabText(index, tab->getTabName());
			}
		}

		void ChatTabWidget::onTabHasUnreadMessages(ChatTab* tab) {
			auto const index = this->indexOf(tab);
			if (index > -1) {
				this->setTabBlinking(index, true);
			}
		}

		void ChatTabWidget::slotCurrentChanged(int index) {
			if (index != -1) {
				ChatTab* const chatTab = dynamic_cast<ChatTab*>(this->widget(index));
				if (chatTab != nullptr) {
					this->setTabBlinking(index, false);
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

		// Overrides for tracking first Tab insertion
		void ChatTabWidget::tabInserted(int index) {
			if (index == 0) {
				QTabBar* tabBar = this->tabBar();
				QWidget* closeButton = tabBar->tabButton(0, QTabBar::ButtonPosition::RightSide);
				if (closeButton != nullptr) {
					tabBar->setTabButton(0, QTabBar::ButtonPosition::RightSide, nullptr);
				}
			}
			QTabWidget::tabInserted(index);
		}

	}
}
