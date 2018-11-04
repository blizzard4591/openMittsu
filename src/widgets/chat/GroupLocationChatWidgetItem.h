#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPLOCATIONCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPLOCATIONCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class GroupLocationChatWidgetItem : public GroupChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupLocationChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupLocationChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblText;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPLOCATIONCHATWIDGETITEM_H_
