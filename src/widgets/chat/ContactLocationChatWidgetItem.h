#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTLOCATIONCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTLOCATIONCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class ContactLocationChatWidgetItem : public ContactChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactLocationChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactLocationChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblLocation;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTLOCATIONCHATWIDGETITEM_H_
