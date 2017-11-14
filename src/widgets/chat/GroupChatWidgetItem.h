#ifndef OPENMITTSU_WIDGETS_GROUPCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_GROUPCHATWIDGETITEM_H_

#include "ChatWidgetItem.h"

#include <memory>

#include "src/dataproviders/BackedGroupMessage.h"
#include "src/dataproviders/BackedGroup.h"

namespace openmittsu {
	namespace widgets {

		class GroupChatWidgetItem : public ChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupChatWidgetItem();
		public slots:
			virtual void showContextMenu(const QPoint& pos) override;
		protected:
			virtual openmittsu::dataproviders::BackedMessage& getMessage() override;
			virtual openmittsu::dataproviders::BackedMessage const& getMessage() const override;

			openmittsu::dataproviders::BackedGroupMessage m_groupMessage;
			std::shared_ptr<openmittsu::dataproviders::BackedGroup> m_group;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_GROUPCHATWIDGETITEM_H_
