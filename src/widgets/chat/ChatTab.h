#ifndef OPENMITTSU_WIDGETS_CHAT_CHATTAB_H_
#define OPENMITTSU_WIDGETS_CHAT_CHATTAB_H_

#include <QWidget>
#include <QString>

#include "src/protocol/ContactId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"

namespace openmittsu {
	namespace widgets {

		class ChatTab : public QWidget {
			Q_OBJECT
		public:
			ChatTab(QWidget* parent = nullptr) : QWidget(parent) {}
			virtual ~ChatTab() {}

			virtual QString getTabName() = 0;
		signals:
			void tabNameChanged(ChatTab* tab);
		protected:
			virtual void internalOnNewMessage(QString const& uuid) = 0;
			virtual void internalOnReceivedFocus() = 0;
			virtual void internalOnLostFocus() = 0;
		public slots :
			virtual void onNewMessage(QString const& uuid) { internalOnNewMessage(uuid); }
			virtual void onReceivedFocus() { internalOnReceivedFocus(); }
			virtual void onLostFocus() { internalOnLostFocus(); }
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CHATTAB_H_
