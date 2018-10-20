#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPAUDIOCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPAUDIOCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupChatWidgetItem.h"

#include "src/widgets/player/Player.h"

namespace openmittsu {
	namespace widgets {

		class GroupAudioChatWidgetItem : public GroupChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupAudioChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupAudioChatWidgetItem();
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

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPAUDIOCHATWIDGETITEM_H_
