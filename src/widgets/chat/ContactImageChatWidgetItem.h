#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTIMAGECHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTIMAGECHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactChatWidgetItem.h"
#include "src/widgets/ClickAwareLabel.h"

namespace openmittsu {
	namespace widgets {

		class ContactImageChatWidgetItem : public ContactChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactImageChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactImageChatWidgetItem();
		public slots:
			virtual void onMessageDataChanged() override;

			virtual void onImageHasBeenClicked();
		protected:
			virtual void copyToClipboard() override;
		private:
			ClickAwareLabel* m_lblImage;
			QLabel* m_lblCaption;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTIMAGECHATWIDGETITEM_H_
