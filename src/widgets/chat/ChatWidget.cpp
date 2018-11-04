#include "src/widgets/chat/ChatWidget.h"
#include "ui_chatwidget.h"

#include <QString>
#include <QSpacerItem>
#include <QScrollBar>
#include <QPalette>
#include <QResizeEvent>
#include <QTimer>

#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		ChatWidget::ChatWidget(QWidget* parent) : QScrollArea(parent), m_ui(new Ui::ChatWidget), m_topLayout(nullptr), m_items(), m_isActive(false), m_hasUnreadMessage(false), m_unreadMessagesTimer(this) {
			m_ui->setupUi(this);

			OPENMITTSU_CONNECT(&m_unreadMessagesTimer, timeout(), this, onUnreadMessagesTimerExpired());

			this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
			this->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

			QSizePolicy selfSizePolicy = this->sizePolicy();
			selfSizePolicy.setHeightForWidth(true);
			this->setSizePolicy(selfSizePolicy);
			this->setWidgetResizable(true);

			QSizePolicy contentsSizePolicy = m_ui->scrollAreaWidgetContents->sizePolicy();
			contentsSizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
			contentsSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
			contentsSizePolicy.setHeightForWidth(true);
			m_ui->scrollAreaWidgetContents->setSizePolicy(contentsSizePolicy);
			m_ui->scrollAreaWidgetContents->setMinimumWidth(400);

			this->setWidget(m_ui->scrollAreaWidgetContents);

			m_topLayout = new QVBoxLayout();
			m_ui->scrollAreaWidgetContents->setLayout(m_topLayout);

			QPalette myPalette(palette());
			myPalette.setColor(QPalette::Background, Qt::white);
			this->setAutoFillBackground(true);
			this->setPalette(myPalette);

			this->m_unreadMessagesTimer.setSingleShot(false);
		}

		ChatWidget::~ChatWidget() {
			delete m_ui;
		}

		void ChatWidget::scrollToBottom() {
			this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		}

		void ChatWidget::resizeEvent(QResizeEvent* event) {
			// Call the super implementation
			QScrollArea::resizeEvent(event);

			/*const int newInnerWidth = ((int)(0.8 * ((double)this->width())));
			//qDebug() << "resizeEvent: innerWidth = " << newInnerWidth;
			for (int i = 0; i < items.size(); ++i) {
				items.at(i)->setWidthHint(newInnerWidth);
			}*/
			if (event->oldSize().width() != event->size().width()) {
				informAllOfSize();
			}
			this->updateGeometry();
		}

		void ChatWidget::informAllOfSize() {
			int newWidth = m_ui->scrollAreaWidgetContents->width() - 110;
			if (newWidth < 300) {
				newWidth = 300;
			}
			int totalHeight = (6 * m_items.size());

			// Width changed, so recalculate all children
			for (int i = 0; i < m_items.size(); ++i) {
				m_items.at(i)->setOptimalSizeHint(newWidth);
				totalHeight += m_items.at(i)->heightForWidth(newWidth);
			}
			//contents->setMinimumHeight(totalHeight);
			m_ui->scrollAreaWidgetContents->setMinimumHeight(totalHeight);
		}

		void ChatWidget::addItem(ChatWidgetItem* item) {
			OPENMITTSU_CONNECT(item, messageDeleted(ChatWidgetItem*), this, onItemMessageDeleted(ChatWidgetItem*));
			QHBoxLayout* hboxLayout = new QHBoxLayout();
			QDateTime compareTime = QDateTime::currentDateTime();

			if (item->isMessageFromUs()) {
				hboxLayout->addWidget(item, 0, Qt::AlignRight);
				item->setBackgroundColorAndPadding(QStringLiteral("#f7ffe8"), 5);
			} else {
				hboxLayout->addWidget(item, 0, Qt::AlignLeft);
				item->setBackgroundColorAndPadding(QStringLiteral("white"), 5);
			}

			int index = 0;
			for (int i = m_items.size() - 1; i >= 0; --i) {
				if (*m_items.at(i) <= *item) {
					index = i + 1;
					break;
				}
			}

			m_topLayout->insertLayout(index, hboxLayout);
			m_items.insert(index, item);
			m_itemToLayoutMap.insert(item, hboxLayout);

			this->update();
			informAllOfSize();
			this->updateGeometry();

			// Calling the meta object invoke() does not work here as the geometry has not yet been updated.
			//QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
			QTimer::singleShot(50, Qt::TimerType::CoarseTimer, this, SLOT(scrollToBottom()));

			if (m_isActive) {
				if (QApplication::activeWindow() != nullptr) {
					item->setWasReadByUs();
				} else {
					m_hasUnreadMessage = true;
					m_unreadMessagesTimer.setInterval(1000);
					m_unreadMessagesTimer.start();
				}
			} else {
				emit hasUnreadMessages();
			}
		}

		void ChatWidget::markMessagesAsRead() {
			auto it = m_items.begin();
			auto end = m_items.end();
			for (; it != end; ++it) {
				(*it)->setWasReadByUs();
			}
			m_hasUnreadMessage = false;
		}

		void ChatWidget::setIsActive(bool isActive) {
			this->m_isActive = isActive;

			if (m_isActive) {
				markMessagesAsRead();
			}
		}

		void ChatWidget::onUnreadMessagesTimerExpired() {
			if (m_isActive && (QApplication::activeWindow() != nullptr)) {
				markMessagesAsRead();
			}

			if (!m_hasUnreadMessage) {
				m_unreadMessagesTimer.stop();
			}
		}

		void ChatWidget::onItemMessageDeleted(ChatWidgetItem* item) {
			auto it = m_itemToLayoutMap.find(item);
			if (it != m_itemToLayoutMap.end()) {
				OPENMITTSU_DISCONNECT(item, messageDeleted(ChatWidgetItem*), this, onItemMessageDeleted(ChatWidgetItem*));
				QHBoxLayout* layout = it.value();
				m_topLayout->removeItem(layout);
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
				m_items.removeAll(item);
#else
				for (int i = m_items.size() - 1; i >= 0; --i) {
					if (m_items.at(i) == item) {
						m_items.remove(i);
					}
				}
#endif

				delete item;
				delete layout;

				this->update();
				informAllOfSize();
				this->updateGeometry();
			}
		}

	}
}
