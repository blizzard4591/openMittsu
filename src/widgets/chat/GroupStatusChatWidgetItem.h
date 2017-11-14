#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPSTATUSCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPSTATUSCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		class GroupStatusChatWidgetItem : public GroupChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupStatusChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupStatusChatWidgetItem();

		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;
		private:
			QLabel* m_lblStatus;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPSTATUSCHATWIDGETITEM_H_
