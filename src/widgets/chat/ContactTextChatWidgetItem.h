#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTTEXTCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTTEXTCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class ContactTextChatWidgetItem : public ContactChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactTextChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactTextChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblText;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTTEXTCHATWIDGETITEM_H_
