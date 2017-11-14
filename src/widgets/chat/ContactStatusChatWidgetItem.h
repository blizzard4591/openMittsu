#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTSTATUSCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTSTATUSCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class ContactStatusChatWidgetItem : public ContactChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactStatusChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactStatusChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblStatus;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTSTATUSCHATWIDGETITEM_H_
