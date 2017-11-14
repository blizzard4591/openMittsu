#ifndef OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_

#include <QSet>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QStringList>
#include <QMutex>
#include <cstdint>

#include "src/dataproviders/MessageSource.h"
#include "src/messages/contact/ReceiptMessageContent.h"
#include "src/protocol/UniqueMessageIdGenerator.h"
#include "src/tasks/FileDownloaderCallbackTask.h"
#include "src/utility/Location.h"
#include "src/widgets/chat/ChatTab.h"
#include "src/widgets/chat/ChatWidgetItem.h"

namespace Ui {
class SimpleChatTab;
}

namespace openmittsu {
	namespace widgets {

		class SimpleChatTab : public ChatTab {
			Q_OBJECT
		public:
			explicit SimpleChatTab(QWidget* parent = nullptr);
			virtual ~SimpleChatTab();
		public slots:
			void btnInputSendOnClick();
			void btnSendImageOnClick();
			void btnMenuOnClick();

			void ctxMenuImageFromFileOnClick();
			void ctxMenuImageFromUrlOnClick();
			void ctxMenuImageFromClipboardOnClick();

			void edtInputOnReturnPressed();
			void edtInputOnTextEdited();
			void typingTimerOnTimer();
			void scrollDownChatWidget();

			void emojiDoubleClicked(QString const& emoji);

			void fileDownloaderCallbackTaskFinished(openmittsu::tasks::CallbackTask* callbackTask);
		protected slots:
			void loadLastNMessages();
		protected:
			virtual bool sendText(QString const& text) = 0;
			virtual bool sendImage(QByteArray const& image, QString const& caption) = 0;
			virtual bool sendLocation(openmittsu::utility::Location const& location) = 0;
			virtual void sendUserTypingStatus(bool isTyping) = 0;

			virtual void setStatusLine(QString const& newStatus);
			virtual void setDescriptionLine(QString const& newDescription);
			virtual void setMessageCount(int messageCount);

			virtual openmittsu::dataproviders::MessageSource& getMessageSource() = 0;

			virtual void internalOnReceivedFocus() override;
			virtual void internalOnLostFocus() override;

			virtual bool canUserAgree() const = 0;
			void addChatWidgetItem(ChatWidgetItem* item);

			Ui::SimpleChatTab* m_ui;
			QSet<QString> m_knownUuids;
			QMutex m_knownUuidsMutex;
		private:
			// Handling for Typing Notifications
			bool m_isTyping;
			QTimer m_typingTimer;

			void prepareAndSendImage(QImage const& image);
			void prepareAndSendImage(QByteArray const& imageData);
		};

	}
}

#endif // OPENMITTSU_WIDGETS_SIMPLECHATTAB_H_
