#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPTEXTCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPTEXTCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class GroupTextChatWidgetItem : public GroupChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupTextChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupTextChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblText;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPTEXTCHATWIDGETITEM_H_
