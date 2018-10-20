#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTAUDIOCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTAUDIOCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactChatWidgetItem.h"

#include "src/widgets/player/Player.h"

namespace openmittsu {
	namespace widgets {

		class ContactAudioChatWidgetItem : public ContactChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactAudioChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactAudioChatWidgetItem();

			
		public slots:
			virtual void onMessageDataChanged() override;

		protected:
			virtual void copyToClipboard() override;
		private:
			Player* m_player;
			QLabel* m_lblCaption;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTAUDIOCHATWIDGETITEM_H_
